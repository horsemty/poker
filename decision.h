#ifndef DECISION_H_INCLUDED
#define DECISION_H_INCLUDED

#include<bits/stdc++.h>
#include"CLASS.h"

#define For(i,a,b) for (int i = (a); i <= (b); i++)
#define Cor(i,a,b) for (int i = (a); i >= (b); i--)
#define rep(i,a) for (int i = 0; i < a; i++)
#define Fill(a,b) memset(a,b,sizeof(a))
#define INF 1048576

/*
double win(double m,double n,Normal *F)
{
    /*
    const double min_prob = 0.05;
    const double max_prob = 0.95;
    double fm,fz,prob;

    if(n>0.7)fm = F->probCalc((1-n)*0.4)+0.5;
    else if(n<0.3)fm = F->probCalc(n*0.4)+0.5;
    else fm = 1.0;

    if(m>n)
    {
        if(n>=0.3)fz = F->probCalc((m-n)*0.4)+0.5;
        else fz = F->probCalc((m-n)*0.4)+F->probCalc(n*0.4);
    }
    else
    {
        if(n<=0.7)fz = F->probCalc((n-m)*0.4)+0.5;
        else fz = F->probCalc((n-m)*0.4)+F->probCalc((1-n)*0.4);
    }

    prob = fz/fm;
    cout<<prob<<endl;
    if(prob > max_prob) prob = max_prob;
    if(prob < min_prob) prob = min_prob;
    return prob;

    double w=2*(1-fabs(m-n))*fabs(m-n);
    if(w>0.5)
        return m>n?1:0;
    else
        return m>n?0.5+w:0.5-w;
}
*/

double win(int mode, double myehs, double Pcall, double Praise, Player m, double x)
{
    const double min_prob = 0.05;
    const double max_prob = 0.95;
    double h,prob = myehs;

    if(myehs < 0.7)return 0;

    /*if(mode == 1)//call
    {
        h=2/(1-Pcall);
        if(myehs < Praise)
            prob = h*(myehs-Pcall)*(myehs-Pcall)/2/(Praise-Pcall);
        else
            prob = 1-h*(1-myehs)*(1-myehs)/2/(1-Praise);
    }*/
    if(mode == 1||mode == 2||mode == 3)
    {
        if(myehs < Pcall)
            prob = min_prob;
        else if(myehs < Praise)
            prob = (myehs-Pcall)*(myehs-Pcall)/(Praise-Pcall)/(2-Praise-Pcall);
        else
            prob = (myehs*2-Praise-Pcall)/(2-Praise-Pcall);
        if(mode == 1) prob = 0.2 + prob * 0.8;
    }
    if(mode == 4)//fold
    {
        return 1.0;
    }
    if(mode == 5||mode == 6||mode == -1)//check
    {
        prob = myehs;
    }

    if(prob > max_prob) prob = max_prob;
    if(prob < min_prob) prob = min_prob;
    return prob;
}

double calcProb(State now, Player m, double EHS)
{
    double winProb=1;
    for(int i=1;i<=now.totPlayer;i++)
    {
        if(now.player[i].pid == m.pid)continue;
        if(now.player[i].identity != -1)
        {
            //winProb *= win(EHS,now.player[i].prob,normal);
            double winthis = win(now.player[i].act,EHS,0.7,0.9,m,now.player[i].bet/(now.player[i].bet+now.player[i].jetton));
            winProb *= winthis;
            std::cerr<<"winProb:\n";
            std::cerr<<now.player[i].pid<<": "<<winthis<<std::endl;
        }
    }
    /*
    double opNum=now.opNum;
    double myP = (m.bet + now.maxBet);
    if((m.bet + now.maxBet + m.jetton)>0)myP /=(m.bet + now.maxBet + m.jetton);
    double Prob2 =1-0.5*myP*myP*myP;
    winProb *= Prob2;
    */

    return winProb;
}

double Expectation(State now, Player m, double EHS, int raiseNum, double winProb)
{
    double bet=m.bet;
    double maxBet=now.maxBet;
    double pot=0;
    Normal *normal = new Normal;
    normal->init();

    if(raiseNum == 0)
    {
        for(int i=1;i<=now.totPlayer;i++)
        {
            if(now.player[i].pid == m.pid)continue;
            if(now.player[i].identity == -1)
                pot += now.player[i].bet;
            else
            {
                if(now.player[i].bet == bet + maxBet)
                    pot += bet + maxBet;
                else
                    pot += bet;
                double nadd = bet + maxBet - now.player[i].bet;
                if(nadd<now.player[i].jetton) nadd = now.player[i].jetton;
                double x0;
				if (now.player[i].jetton == 0)
					x0 = 0.0;
				else
			   		x0 = nadd/now.player[i].jetton;
				double prob;

                if((now.player[i].bet + nadd) / (now.player[i].jetton + now.player[i].bet) < 0.1)
                    prob = 0.8;
                else
                    prob = 0.875*(1.0-x0) + 0.1;
                pot += prob * nadd;
            }
        }
    }
    else
    {
        for(int i=1;i<=now.totPlayer;i++)
        {
            if(now.player[i].pid == m.pid)continue;
            if(now.player[i].identity == -1)
                pot += now.player[i].bet;
            else
            {
                if(now.player[i].bet == bet + maxBet)
                    pot += maxBet;
                else
                    pot += bet;
                double nadd = bet + maxBet + now.player[i].bet + raiseNum;
                if(nadd<now.player[i].jetton) nadd = now.player[i].jetton;
                double x0;
				if (now.player[i].jetton == 0)
					x0 = 0.0;
				else
			   		x0 = nadd/now.player[i].jetton;
				double prob;

                if((now.player[i].bet + nadd) / (now.player[i].jetton + now.player[i].bet) < 0.1)
                    prob = 0.8;
                else
                    prob = 0.875*(1.0-x0) + 0.1;
                pot += prob * nadd;
            }
        }
    }
    /*
    double opNum=now.opNum;
    double myP = (m.bet + maxBet);
    if((m.bet + maxBet + m.jetton)>0)myP /=(m.bet + maxBet + m.jetton);
    double Prob2 =1-0.5*myP*myP*myP;
    winProb *= Prob2;
    */

    std::string str = "decision";
	str += char(m.pid % 10 + '0');
	FILE *f = fopen(str.c_str(), "a");
	fprintf(f, "%.4f %.4f\n", EHS, winProb);
	fclose(f);

    double Exp=(pot+bet+maxBet+raiseNum)*winProb-bet-maxBet-raiseNum;
    return Exp;
}
namespace Two_cards
{
    class Decision
    {
    public:
        int Fold,Raise;
    }dec[10];
    int Sklansky[15][15]={{1,1,2,2,3,5,5,5,5,5,5,5,5},
                          {2,1,2,3,4,7,7,7,7,7,7,7,7},
                          {3,4,1,3,4,5,7,9,9,9,9,9,9},
                          {4,5,5,1,3,4,6,6,9,9,9,9,9},
                          {6,6,6,5,2,4,5,7,9,9,9,9,9},
                          {8,8,8,7,7,3,4,5,8,9,9,9,9},
                          {9,9,9,8,8,7,4,5,6,8,9,9,9},
                          {9,9,9,9,9,9,8,5,5,6,8,9,9},
                          {9,9,9,9,9,9,9,8,5,6,7,9,9},
                          {9,9,9,9,9,9,9,9,8,6,6,7,9},
                          {9,9,9,9,9,9,9,9,9,8,7,7,8},
                          {9,9,9,9,9,9,9,9,9,9,9,7,8},
                          {9,9,9,9,9,9,9,9,9,9,9,9,7}};
    void Init()
    {
        dec[1]={20,30};
        dec[2]={20,15};
        dec[3]={20,5};
        dec[4]={20,2};
        dec[5]={15,2};
        dec[6]={10,2};
        dec[7]={5,2};
        dec[8]={2,2};
        dec[9]={0,0};
    }
    int Sklansky_point(Card a, Card b)
    {
        int p1,p2;
        p1=14-a.number;
        p2=14-b.number;
        if(a.color==b.color)
            if(p1>p2)std::swap(p1,p2);
        if(a.color!=b.color)
            if(p1<p2)std::swap(p1,p2);
        return Sklansky[p1][p2];
    }
    int Choose(State now, Player m)
    {
        int jetton = m.jetton;
        int mypoint = Sklansky_point(m.card[0],m.card[1]);
        int smallBlind = now.smallBlind;
        int maxBet = now.maxBet;
        if(m.bet + maxBet < dec[mypoint].Raise*smallBlind)
        {
            if(maxBet > jetton)return 3;
            else return dec[mypoint].Raise*smallBlind-maxBet;
        }
        else if(maxBet==0) return 5;
        else
        {
            if(m.bet + maxBet > dec[mypoint].Fold * smallBlind) return 4;
            else return 1;
        }
    }
};

class Rank
{
	private:
		std::map<int, int> OT;
		int flushes[8000], unique5[8000];
		const int prime[15] = {0, 0, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};
		int hashCard(int num, int col)
		{
			int x = prime[num];
			x += num << 8;
			x += 1 << (12 + col);
			x += 1 << (num - 2 + 16);
			return x;
		}
	public:
		void init()
		{
			FILE *f = fopen("flushes.out", "r");
			int a, b;
			while (fscanf(f, "%d %d", &a, &b) != EOF)
				flushes[b] = a;
			fclose(f);
			f = fopen("unique5.out", "r");
			while (fscanf(f, "%d %d", &a, &b) != EOF)
				unique5[b] = a;
			fclose(f);
			f = fopen("othertype.out", "r");
			while (fscanf(f, "%d %d", &a, &b) != EOF)
				OT[b] = a;
			fclose(f);
		}
		int rank(Card *C)
		{
			int c[6];
			rep(i, 5)
				c[i] = hashCard(C[i].number, C[i].color);
			int r = c[0];
			For(i, 1, 4)
				r &= c[i];
			r &= 0xF000;
			int q = 0;
			For (i, 0, 4)
				q |= c[i];
			q >>= 16;
			if (r != 0)
			{
				return flushes[q];
			}
			else
			{
				int num = __builtin_popcount(q);
				if (num == 5)
				{
					return unique5[q];
				}
				else
				{
					int x = 1;
					For(i, 0, 4)
						x *= (c[i] & 0xff);
					return OT[x];
				}
			}
		}
};

int Card2num(Card k)
{
    return k.color*13+k.number - 2;
}
Card Num2card(int k)
{
    Card tmp;
    tmp.color=k/13;
    tmp.number=k%13 + 2;
    return tmp;
}

int dfsend;
Card dfsCards[10];
int p;
int dfs(int pos,Card *C,int num,Rank *Rk)
{
    if(pos==dfsend)
    {
        if(num==5)return Rk->rank(dfsCards);
        else return 1e9;
    }
    int res=1e9,tmp;
    dfsCards[p++]=C[pos];
    tmp=dfs(pos+1,C,num+1,Rk);
    res=std::min(tmp,res);
    p--;
    tmp=dfs(pos+1,C,num,Rk);
    res=std::min(tmp,res);
    return res;
}

int S_Five(Card *C, int mode, Rank *Rk)
{
	switch(mode)
	{
        case 1://Max of six
        {
            dfsend=6;p=0;
            return dfs(0,C,0,Rk);
        }
        case 2://Max of seven
        {
            dfsend=7;p=0;
            return dfs(0,C,0,Rk);
        }
	};
	return 0;
}


double raiseX(State now, Player m, double Exp0, double ehs, double sumProb)//Not Completed----------------------------------------------------------
{
	//double sumProb=1;
	Normal *normal = new Normal;
    normal->init();
    //double sumPlayer,double sumPlayer
	for(int i=1;i<=now.totPlayer;i++)
	{
		if(now.player[i].pid == m.pid)continue;
		if(now.player[i].identity == -1)continue;
		else
		{
			double nadd = m.bet + now.maxBet + now.player[i].bet;
			if(nadd<now.player[i].jetton) nadd = now.player[i].jetton;
			//sumProb *= win(m.act,ehs,0.7,0.2,m);
			//if(now.player[i].prob > 0.8)sumProb *= win(ehs,now.player[i].prob,normal);
			//else sumProb *= win(ehs,0.8,normal);
		}
	}

	FILE *f = fopen("sumprob.out", "a");
	fprintf(f,"%.4f\n",sumProb);
	fclose(f);

	if (sumProb < 0.6)return 0;
	if (sumProb > 0.9)return m.jetton;
	else return (double)m.jetton*(sumProb-0.5)*ehs*2;
}

int remainCards[55],remainCardnum;
void Shuffle()
{
    for(int i=0;i<remainCardnum;i++)
        std::swap(remainCards[i],remainCards[rand()%remainCardnum]);
}

namespace Five_cards
{
    double Hand_Strength(State now, Player m, Rank *Rk)
    {
        double ahead=0,tied=0,behind=0;
        Card ourCards[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2]};
        int ourRank = Rk->rank(ourCards);
        int visit[55]={0};

        for(int i=0;i<2;i++)
            visit[Card2num(m.card[i])]=true;
        for(int i=0;i<now.numFlop;i++)
            visit[Card2num(now.flop[i])]=true;

        for(int i=0;i<52;i++)
        {
            if(visit[i])continue;
            visit[i]=true;
            for(int j=i+1;j<52;j++)
            {
                if(visit[j])continue;
                visit[j]=true;

                Card opponentCards[10]={Num2card(i),Num2card(j),now.flop[0],now.flop[1],now.flop[2]};
                int opponentRank = Rk->rank(opponentCards);

                if(ourRank<opponentRank)ahead++;
                if(ourRank==opponentRank)tied++;
                if(ourRank>opponentRank)behind++;

                visit[j]=false;
            }
            visit[i]=false;
        }
        double handstrength = (ahead+tied/2)/(ahead+tied+behind);

        return handstrength;
    }
    std::pair<double,double> Hand_Potential(State now, Player m, Rank *Rk)//Not Completed----------------------------------------------------------
    {
		//std::string str = "decision";
		//str += char(m.pid % 10 + '0');
		//FILE *f = fopen(str.c_str(), "a");
        Card ourCards[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2]};
        int ourRank = Rk->rank(ourCards);
        int times=10000;
        int opNum = now.opNum;
        int visit[55]={0};
        double HP[3][3]={0};

        for(int i=0;i<2;i++)
            visit[Card2num(m.card[i])]=true;
        for(int i=0;i<now.numFlop;i++)
            visit[Card2num(now.flop[i])]=true;

        remainCardnum = 0;
        for(int i=0;i<52;i++)
            if(!visit[i])remainCards[remainCardnum++] = i;

        int opCard[8][2];

        while(times--)
        {
            int pos=0,idx=0,nidx=0;
            Shuffle();
            for(int i=0;i<opNum;i++)
            {
                for(int c=0;c<2;c++)
                    opCard[i][c]=remainCards[pos++];
                Card opponentCards[10]={Num2card(opCard[i][0]),Num2card(opCard[i][1]),now.flop[0],now.flop[1],now.flop[2]};
                int opponentRank = Rk->rank(opponentCards);
                if(opponentRank < ourRank) idx = std::max(2, idx);//Behind
                if(opponentRank == ourRank) idx = std::max(idx, 1);//Tied
            }
            int turn = remainCards[pos++], river = remainCards[pos++];

            Card ourCards7[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2],Num2card(turn),Num2card(river)};
            int ourRank7=S_Five(ourCards7,2,Rk);

            for(int i=0;i<opNum;i++)
            {
                Card opponentCards7[10]={Num2card(opCard[i][0]),Num2card(opCard[i][1]),now.flop[0],now.flop[1],now.flop[2],Num2card(turn),Num2card(river)};
                int opponentRank7=S_Five(opponentCards7,2,Rk);
                if(opponentRank7 < ourRank7) nidx = std::max(2, nidx);
                if(opponentRank7 == ourRank7) nidx = std::max(1, nidx);
            }
            HP[idx][nidx]++;
        }
		/*
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
				fprintf(f, "%f ", HP[i][j]);
			fprintf(f, "\n");
		}
		fclose(f);*/
        double PPot = (HP[2][0]+HP[2][1]/2+HP[1][0]/2);
		if ((HP[2][0]+HP[2][1]+HP[2][2]+HP[1][2]+HP[1][1]+HP[1][0]) != 0)
			PPot /= (HP[2][0]+HP[2][1]+HP[2][2]+HP[1][2]+HP[1][1]+HP[1][0]);
        double NPot = (HP[0][2]+HP[1][2]/2+HP[0][1]/2);
		if ((HP[0][0]+HP[0][1]+HP[0][2]+HP[1][2]+HP[1][1]+HP[1][0]) != 0)
			NPot /= (HP[0][0]+HP[0][1]+HP[0][2]+HP[1][2]+HP[1][1]+HP[1][0]);
        return std::make_pair(PPot,NPot);
    }
    double EHS(State now, Player m, Rank *Rk)
    {
		/*
		std::string str = "decision";
		str += char(m.pid % 10 + '0');
		FILE *f = fopen(str.c_str(), "a");
		*/
        int opNum=now.opNum;
        double HS = Hand_Strength(now, m, Rk);
        std::pair<double,double> Pot = Hand_Potential(now, m, Rk);
        double PPot = Pot.first, NPot = Pot.second;
        double ehs = HS * (1-NPot) + (1-HS) * PPot;
		//fprintf(f, "%.4f %.4f %.4f %.4f\n", HS, PPot, NPot, ehs);
		//fclose(f);
        return ehs;
    }
    /*double EHS(State now, Player m, Rank *Rk)
    {
        int opNum=now.opNum;
        double HS = Hand_Strength(now, m, Rk);
        return pow(HS,(double)opNum);
    }*/
    int Choose(State now, Player m, Rank *Rk)//Not Completed----------------------------------------------------------
    {
		/*
		std::string str = "decision";
		str += char(m.pid % 10 + '0');
		FILE *f = fopen(str.c_str(), "a");
		//fprintf(f, "%d: %.4f %d\n", m.pid, EHS(now, m, Rk), now.opNum);
		fclose(f);
		*/
		double ehs = EHS(now, m, Rk);
		double winProb = calcProb(now, m, ehs);
        double Exp = Expectation(now, m, ehs, 0, winProb);
        if(Exp < 0)
		{
			if(now.maxBet)
				return 4;
			else
				return 5;
		}
        double raiseBlind = raiseX(now, m, Exp, ehs, winProb);
        FILE *f = fopen("myraisex.out","a");
        fprintf(f,"Flop: %4f\n",raiseBlind);
        fclose(f);
        //double Exp2 = Expectation(now, m, ehs, raiseBlind);
        //if(Exp2 - Exp > 0)
        if(raiseBlind > 0)
        {
            if(now.maxBet+raiseBlind > m.jetton)return 3;
            else return (int)raiseBlind;
        }
        else if(now.maxBet)
        {
            if(now.maxBet > m.jetton)return 3;
            else return 1;
        }
        return 5;
    }
};



namespace Six_cards
{
    double Hand_Strength(State now, Player m, Rank *Rk)
    {
        double ahead=0,tied=0,behind=0;
        Card ourCards[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2],now.flop[3]};
        int ourRank = S_Five(ourCards,1,Rk);
        int visit[55]={0};

        for(int i=0;i<2;i++)
            visit[Card2num(m.card[i])]=true;
        for(int i=0;i<now.numFlop;i++)
            visit[Card2num(now.flop[i])]=true;

        for(int i=0;i<52;i++)
        {
            if(visit[i])continue;
            visit[i]=true;
            for(int j=i+1;j<52;j++)
            {
                if(visit[j])continue;
                visit[j]=true;

                Card opponentCards[10]={Num2card(i),Num2card(j),now.flop[0],now.flop[1],now.flop[2],now.flop[3]};
                int opponentRank = S_Five(opponentCards,1,Rk);

                if(ourRank<opponentRank)ahead++;
                if(ourRank==opponentRank)tied++;
                if(ourRank>opponentRank)behind++;

                visit[j]=false;
            }
            visit[i]=false;
        }
        double handstrength = (ahead+tied/2)/(ahead+tied+behind);

        return handstrength;
    }
    std::pair<double,double> Hand_Potential(State now, Player m, Rank *Rk)//Not Completed----------------------------------------------------------
    {
        Card ourCards[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2],now.flop[3]};
        int ourRank = S_Five(ourCards,1,Rk);
        int times=10000;
        int opNum = now.opNum;
        int visit[55]={0};
        double HP[3][3]={0};

        for(int i=0;i<2;i++)
            visit[Card2num(m.card[i])]=true;
        for(int i=0;i<now.numFlop;i++)
            visit[Card2num(now.flop[i])]=true;

        remainCardnum = 0;
        for(int i=0;i<52;i++)
            if(!visit[i])remainCards[remainCardnum++] = i;

        int opCard[8][2];

        while(times--)
        {
            int pos=0,idx=0,nidx=0;
            Shuffle();
            for(int i=0;i<opNum;i++)
            {
                for(int c=0;c<2;c++)
                    opCard[i][c]=remainCards[pos++];
                Card opponentCards[10]={Num2card(opCard[i][0]),Num2card(opCard[i][1]),now.flop[0],now.flop[1],now.flop[2],now.flop[3]};
                int opponentRank = S_Five(opponentCards,1,Rk);
                if(opponentRank < ourRank) idx = std::max(idx, 2);
                if(opponentRank == ourRank) idx = std::max(idx, 1);
            }
            int river = remainCards[pos++];

            Card ourCards7[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2],now.flop[3],Num2card(river)};
            int ourRank7=S_Five(ourCards7,2,Rk);

            for(int i=0;i<opNum;i++)
            {
                Card opponentCards7[10]={Num2card(opCard[i][0]),Num2card(opCard[i][1]),now.flop[0],now.flop[1],now.flop[2],now.flop[3],Num2card(river)};
                int opponentRank7=S_Five(opponentCards7,2,Rk);
                if(opponentRank7 < ourRank7) nidx = std::max(nidx, 2);
                if(opponentRank7 == ourRank7) nidx = std::max(nidx, 1);
            }
            HP[idx][nidx]++;
        }
        double PPot = (HP[2][0]+HP[2][1]/2+HP[1][0]/2);
		if ((HP[2][0]+HP[2][1]+HP[2][2]+HP[1][2]+HP[1][1]+HP[1][0]) != 0)
			PPot /= (HP[2][0]+HP[2][1]+HP[2][2]+HP[1][2]+HP[1][1]+HP[1][0]);
        double NPot = (HP[0][2]+HP[1][2]/2+HP[0][1]/2);
		if ((HP[0][0]+HP[0][1]+HP[0][2]+HP[1][2]+HP[1][1]+HP[1][0]) != 0)
			NPot /= (HP[0][0]+HP[0][1]+HP[0][2]+HP[1][2]+HP[1][1]+HP[1][0]);
        return std::make_pair(PPot,NPot);
    }
    double EHS(State now, Player m, Rank *Rk)
    {
        int opNum=now.opNum;
        double HS = Hand_Strength(now, m, Rk);
        std::pair<double,double> Pot = Hand_Potential(now, m, Rk);
        double PPot = Pot.first, NPot = Pot.second;
        double ehs = HS * (1-NPot) + (1-HS) * PPot;
        return ehs;
    }
    /*double EHS(State now, Player m, Rank *Rk)
    {
        int opNum=now.opNum;
        double HS = Hand_Strength(now, m, Rk);
        return pow(HS,(double)opNum);
    }*/
    int Choose(State now, Player m, Rank *Rk)//Not Completed----------------------------------------------------------
    {
		double ehs = EHS(now, m, Rk);
		double winProb = calcProb(now, m, ehs);
        double Exp = Expectation(now, m, ehs, 0, winProb);
        if(Exp < 0)
		{
			if(now.maxBet)
				return 4;
			else
				return 5;
		}
        double raiseBlind = raiseX(now, m, Exp, ehs, winProb);

        FILE *f = fopen("myraisex.out","a");
        fprintf(f,"Turn: %4f\n",raiseBlind);
        fclose(f);

        //double Exp2 = Expectation(now, m, ehs, raiseBlind);
        //if(Exp2 - Exp > 0)
        if(raiseBlind > 0)
        {
            if(now.maxBet+raiseBlind > m.jetton)return 3;
            else return (int)raiseBlind;
        }
        else if(now.maxBet)
        {
            if(now.maxBet > m.jetton)return 3;
            else return 1;
        }
        return 5;
    }
};

namespace Seven_cards
{
    double Hand_Strength(State now, Player m, Rank *Rk)
    {
        double ahead=0,tied=0,behind=0;
        Card ourCards[10]={m.card[0],m.card[1],now.flop[0],now.flop[1],now.flop[2],now.flop[3],now.flop[4]};
        int ourRank = S_Five(ourCards,2,Rk);
        int visit[55]={0};

        for(int i=0;i<2;i++)
            visit[Card2num(m.card[i])]=true;
        for(int i=0;i<now.numFlop;i++)
            visit[Card2num(now.flop[i])]=true;

        for(int i=0;i<52;i++)
        {
            if(visit[i])continue;
            visit[i]=true;
            for(int j=i+1;j<52;j++)
            {
                if(visit[j])continue;
                visit[j]=true;

                Card opponentCards[10]={Num2card(i),Num2card(j),now.flop[0],now.flop[1],now.flop[2],now.flop[3],now.flop[4]};
                int opponentRank = S_Five(opponentCards,2,Rk);

                if(ourRank<opponentRank)ahead++;
                if(ourRank==opponentRank)tied++;
                if(ourRank>opponentRank)behind++;

                visit[j]=false;
            }
            visit[i]=false;
        }
        double handstrength = (ahead+tied/2)/(ahead+tied+behind);

        return handstrength;
    }
    double EHS(State now, Player m, Rank *Rk)
    {
        int opNum=now.opNum;
        double HS = Hand_Strength(now, m, Rk);
        return HS;
    }
    int Choose(State now, Player m, Rank *Rk)//Not Completed----------------------------------------------------------
    {
		double ehs = EHS(now, m, Rk);
		double winProb = calcProb(now, m, ehs);
        double Exp = Expectation(now, m, ehs, 0, winProb);
		/*
		std::string str = "decision";
		str += char (m.pid % 10 + '0');
		FILE *f = fopen(str.c_str(), "a");
		fprintf(f, "%.4f\n", Exp);
		fclose(f);
		*/
        if(Exp < 0)
		{
			if(now.maxBet)
				return 4;
			else
				return 5;
		}
        double raiseBlind = raiseX(now, m, Exp, ehs, winProb);

        FILE *f = fopen("myraisex.out","a");
        fprintf(f,"River: %4f\n",raiseBlind);
        fclose(f);

        //double Exp2 = Expectation(now, m, ehs, raiseBlind);
        //if(Exp2 - Exp > 0)
        if(raiseBlind > 0)
        {
			/*
			int r = rand() % 10;
			if (Exp2 - Exp > 0 || r < 2)
			{
				if(now.maxBet+raiseBlind*now.smallBlind > m.jetton)return 3;
				else return raiseBlind * now.smallBlind;
			}
			else if(now.maxBet)
			{
				if(now.maxBet > m.jetton)return 3;
				else return 1;
			}
			 */
			if(now.maxBet+raiseBlind > m.jetton)return 3;
			else return (int)raiseBlind;
        }
        else if(now.maxBet)
        {
            if(now.maxBet > m.jetton)return 3;
            else return 1;
        }
        return 5;
    }
};
#endif // DECISION_H_INCLUDED
