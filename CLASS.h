#ifndef CLASS_H_INCLUDED
#define CLASS_H_INCLUDED

struct Card
{
	int color, number;
	Card(int c, int n): color(c), number(n) {}
	Card() {}
};

struct Action
{
	int name, val;
	/*
		name : 0 means all in
		name : 1 means call
		name : 2 means raise
		name : 3 means all in
		name : 4 means fold
		name : 5 means check
		name : 6 means blind
	*/
	Action(int n, int v): name(n), val(v) {}
};

struct PlayerInfo
{
	struct eachPlayer
	{
		int money, jetton;
		int actionNum[10];
	}P[10];
	std::map<int, int> pMemo;
	int pNum;
	void init(int pid, int _jetton, int _money)
	{
		if (pMemo.find(pid) == pMemo.end())
			pMemo[pid] = ++pNum;
		int t = pMemo[pid];
		P[t].money = _money, P[t].jetton = _jetton;
	}
};
class Player
{
	public:
		int money, numCard, pid, jetton;
		Card card[3];
		int identity; // 0 is normal, 1 is small blind, 2 is big blind, 3 is button
		//std::vector<Action> act; //
		int act;
		int rank, nut, bet;
		double prob;
		/*
			nut : 0 means HIGH_CARD
			nut : 1 means ONE_PAIR
			nut : 2 means TWO_PAIR
			nut : 3 means THREE_OF_A_KIND
			nut : 4 means STRAIGHT
			nut : 5 means FLUSH
			nut : 6 means FULL_HOUSE
			nut : 7 means FOUR_OF_A_KIND
			nut : 8 means STRAIGHT_FLUSH
		 */
		void init(int _pid, int _identity, int _jetton, int _money)
		{
			numCard = 0;
			pid = _pid;
			jetton = _jetton;
			money = _money;
			bet = 0;
			identity = _identity;
			prob = 0.6;
			act = -1;
		}
		void update(int _jetton, int _money, int _action)
		{
			jetton = _jetton;
			money = _money;
			act = _action;
		}
		void addCard(int color, int number)
		{
			card[numCard].color = color, card[numCard].number = number;
			numCard++;
		}
		void addAction(int name, int val)
		{
			act = name;
		}
		void moneyChange(int d)
		{
			jetton -= d;
		}
};
class State
{
	public:
		Card flop[5];
		int numFlop = 0;
		int pot = 0;
		int opNum;
		int bigBlind, smallBlind;
		Player player[9];
		int totPlayer = 0; //renumber the players from 1 to 8
		std::map<int, int> playersNum;
		std::vector<int> subPot;
		int rank[9];
		int maxBet = 0;
		void addPlayer(int pid, int identity, int jetton, int money);
		void addFlop(int color, int number)
		{
			flop[numFlop++] = Card(color, number);
		}
		void addJettonMainPot(int val)
		{
			pot += val;
		}
		void addJettonSubPot(int val)
		{
		}
		void updatePlayer(int pid, int jetton, int money, int bet, int action)
		{
			int pos = playersNum[pid];
			if (action == 4)
				player[pos].identity = -1;
			player[pos].update(jetton, money, action);
			if (bet > maxBet)
				maxBet = bet;
			player[pos].bet = bet;
		}
		void blind(int pid, int bet)
		{
			int pos = playersNum[pid];
			if (player[pos].identity == 1)
				smallBlind = bet;
			else
				bigBlind = bet;
			player[pos].moneyChange(bet);
		}
		void addResult(int pid, int _rank, int nut)
		{
			int pos = playersNum[pid];
			rank[_rank] = pos;
			player[pos].rank = _rank;
			player[pos].nut = nut;
		}
};

class OpponentModel
{
	public:
	int Decision[4][4];
	double probability[4][3];
	double weight[3000];
	bool raised;
	const double low_wt = 0.01;
	const double high_wt = 1.00;

	void InitGame()
	{
		memset(Decision,0,sizeof(Decision));
		memset(probability,0,sizeof(probability));
	}
	void InitState()
	{
		memset(weight,0,sizeof(weight));
	}
	void InitPart()
	{
		raised = 0;
	}
	void init()
	{
		memset(Decision,0,sizeof(Decision));
		memset(probability,0,sizeof(probability));
		raised = 0;
	}
	void Begin_Model_Create()
	{
        probability[0][0]=0.6,probability[0][1]=0.25,probability[0][2]=0.15;
        probability[1][0]=0.4,probability[1][1]=0.5,probability[1][2]=0.1;
        probability[2][0]=0.3,probability[2][1]=0.6,probability[2][2]=0.1;
        probability[3][0]=0.5,probability[3][1]=0.3,probability[3][2]=0.2;
	}

	void Make_Decision(int part, int behavior)
	{
		if(behavior == 2)
		{
			if(raised)return;
			else raised=true;
		}
		Decision[part][behavior]++;
		Decision[part][3]++;
		Begin_Model_Create();
	}

	double Upgrade_weight(int part, int behavior, double x, Player m/*, Rank *Rk, Card *Me, Card *Common*/)
	{
		if(behavior == 2 && !raised)
		{
			raised=true;
			Decision[part][behavior]++;
			Decision[part][3]++;
			Begin_Model_Create();
		}
		else
		{
			if (behavior != 2)
			{
				Decision[part][behavior]++;
				Decision[part][3]++;
				Begin_Model_Create();
			}
		}
		double Pfold=probability[part][0];
		double Pcheck=probability[part][1];
		double Praise=probability[part][2];
		//int raisex=(raisenum/m.jetton);
		if(behavior == 0)return 0;
		else
		{
			if(behavior == 1){
                double x0;
                if(x < 0.1)
                    x0 = Pfold + Pcheck*(0.4 + x * 3);
                else
                    x0 = Pfold + Pcheck*std::min(0.7 + x * 0.6,1.0) + Praise * (0.1+0.7*x);
                return x0;
            }
			else
				if(behavior == 2){
                    double x0;
                    if(x < 0.5)
                        return Pfold + Pcheck * (0.75+0.5*x) + Praise * (0.2+0.7*x);
                    else
                        return Pfold + Pcheck + Praise * (0.2+0.7*x);
                }
		}
	}
};
struct OP
{
	OpponentModel P[8];
	std::map<int, int> pMemo;
	int pNum = 0;
	void init(int pid)
	{
		if (pMemo.find(pid) == pMemo.end())
			P[pNum].init(), pMemo[pid] = pNum++;
	}
	void addAction(int pid, int act, int sta);
	void initRaise()
	{
		for (int i = 0; i < 8; i++)
			P[i].raised = 0;
	}
};
class Normal
{
	public:
	double p[210];
	void init()
	{
		FILE *f = fopen("normal.out", "r");
		int tot = 0;
		while (fscanf(f, "%le", &p[tot++]) != EOF);
		fclose(f);
	}
	double probCalc(double d)
	{
		if (d > 0.5)
			return 1;
		int t = round((d + 0.5) / 0.005);
		return p[t] - 0.5;
	}
};
#endif // CLASS_H_INCLUDED
