#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <sstream>
#include <malloc.h>
#include "CLASS.h"
#include "decision.h"
//using namespace std;
typedef long long LL;
#define For(i,a,b) for (int i = (a); i <= (b); i++)
#define Cor(i,a,b) for (int i = (a); i >= (b); i--)
#define rep(i,a) for (int i = 0; i < a; i++)
#define Fill(a,b) memset(a,b,sizeof(a))

const char *split = "\n";
int m_socket_id = -1;
int my_id;

Player *Me;
PlayerInfo pInfo;
OP opModel;
State state[1001];
int currentState = 0;
Rank *rank = new Rank;

void State::addPlayer(int pid, int identity, int jetton, int money)
{
    totPlayer++;
    playersNum[pid] = totPlayer;
    player[totPlayer].init(pid, identity, jetton, money);
    if (pid == my_id)
    Me = &player[totPlayer];
}
void OP::addAction(int pid, int act, int sta)
{
	/*
	std::string str = "decision";
	str += char(Me->pid % 10 + '0');
	FILE *f = fopen(str.c_str(), "a");
	*/
	if (sta >= 3)
		sta -= 2;
	int t = pMemo[pid];
	int pos = state[currentState].playersNum[pid];
	double prob = 0.0;
	double x = (double)state[currentState].player[pos].bet * 1.0 / (state[currentState].player[pos].bet + state[currentState].player[pos].jetton);
	if (act == 4) //fold
	{
		prob = this->P[t].Upgrade_weight(sta, 0, x, state[currentState].player[pos]);
	}
	if (act == 2 || act == 3) // raise
	{
		prob = this->P[t].Upgrade_weight(sta, 2, x, state[currentState].player[pos]);
	}
	if (act == 1) //call
	{
		prob = this->P[t].Upgrade_weight(sta, 1, x, state[currentState].player[pos]);
	}
	if (act == 5)
		prob = 0.6;
	state[currentState].player[pos].prob = std::max(state[currentState].player[pos].prob,prob);
	/*
	if (act < 5)
		fprintf(f, "pid:%d act:%d sta:%d %.4f\n", pid, act, sta, prob);
	fclose(f);
	*/
}
class Message
{
	private:
		struct inqueryMessage
		{
			std::vector<std::vector<int> > p;
			int num;
		};
		void newGame()
		{
			currentState++;
		}
		void seatMsg(int pid, int jetton, int money, int identity)
		{
			state[currentState].addPlayer(pid, identity, jetton, money);
			opModel.init(pid);
			opModel.initRaise();
		}
		void inquireMsg(inqueryMessage iM)
		{
			for (auto i : iM.p)
			{
				if (state[currentState].player[state[currentState].playersNum[i[0]]].identity != -1)
				{
					int sta = state[currentState].numFlop;
					opModel.addAction(i[0], i[4], sta);
				}
				state[currentState].updatePlayer(i[0], i[1], i[2], i[3], i[4]);
				//pInfo.addAction(i[0], i[4]);
			}
			state[currentState].pot = iM.num;
		}
		void blindMsg(int pid, int bet)
		{
			state[currentState].blind(pid, bet);
			//pInfo.P[pInfo.pMemo[pid]].jetton -= bet;
		}
		void holdCardsMsg(int color, int num)
		{
			Me->addCard(color, num);
		}
		void flopMsg(int color, int num)
		{
			state[currentState].addFlop(color, num);
		}
		void showDownMsg(int rank, int pid, int c0, int num0, int c1, int num1, int nut)
		{
			state[currentState].addResult(pid, rank, nut);
			int pos = state[currentState].playersNum[pid];
			if (pos <= 0 || pos >= 9)
				exit(-1);
			state[currentState].player[pos].addCard(c0, num0);
			state[currentState].player[pos].addCard(c1, num1);
		}
		void potMsg(int pid, int num)
		{
		}
		int getInt(char *s)
		{
			switch (s[0])
			{
				case 'J':
				{
					return 11;
				}
				case 'Q':
				{
					return 12;
				}
				case 'K':
				{
					return 13;
				}
				case 'A':
				{
					return 14;
				}
				default:
				{
					int x;
					sscanf(s, "%d", &x);
					return x;
				}
			}
		}
		void parseInquire()
		{
			char *p;
			p = (char *)malloc(100);
			inqueryMessage iM;
			while (scanf("%s", p) != EOF)
			{
				if (p[0] == '/')
					break;
				if (p[0] == 't')
				{
					int num = 0;
					scanf("%*s %d", &num);
					iM.num = num;
				}
				else
				{
					std::vector<int> player(4, 0);
					char action[20] = {'\0'};
					sscanf(p, "%d", &player[0]);
					scanf("%d %d %d %s", &player[1], &player[2], &player[3], action);
					switch (action[0])
					{
						case 'b':
							{
								player.push_back(6);
								break;
							}
						case 'c':
							{
								if (action[1] == 'a')
									player.push_back(1);
								else
									player.push_back(5);
								break;
							}
						case 'r':
							{
								player.push_back(2);
								break;
							}
						case 'a':
							{
								player.push_back(3);
								break;
							}
						case 'f':
							{
								player.push_back(4);
								break;
							}
					}
					iM.p.push_back(player);
				}
			}
			inquireMsg(iM);
			free(p);
		}
		void parseSeat()
		{
			char *p;
			p = (char *)malloc(100);
			while (scanf("%s", p) != EOF)
			{
				if (p[0] == '/')
					break;
				int id = 0;
				int pid, jetton, money;
				if (p[0] == 's')
				{
					scanf("%*s %d %d %d", &pid, &jetton, &money);
					id = 1;
				}
				if (p[0] == 'b')
				{
					if (p[1] == 'i')
						scanf("%*s"), id = 2;
					else
						id = 3;
					scanf("%d %d %d", &pid, &jetton, &money), id = 3;
				}
				if (p[0] >= '0' && p[0] <= '9')
				{
					id = 0;
					sscanf(p, "%d", &pid);
					scanf("%d %d", &jetton, &money);
				}
				seatMsg(pid, jetton, money, id);
			}
			free(p);
		}
		void parseBlind()
		{
			char *p;
			p = (char *)malloc(100);
			while (scanf("%s", p) != EOF)
			{
				if (p[0] == '/')
					break;
				int pid, bet;
				sscanf(p, "%d:", &pid);
				scanf("%d", &bet);
				blindMsg(pid, bet);
			}
			free(p);
		}
		void parseHold()
		{
			char *p;
			p = (char *)malloc(100);
			while (scanf("%s", p) != EOF)
			{
				if (p[0] == '/')
					break;
				int color = 0;
				if (strstr(p, "SPADES"))
					color = 0;
				if (strstr(p, "HEARTS"))
					color = 1;
				if (strstr(p, "CLUBS"))
					color = 2;
				if (strstr(p, "DIAMONDS"))
					color = 3;
				scanf("%s", p);
				int num = getInt(p);
				holdCardsMsg(color, num);
			}
			free(p);
		}
		void parseFlop()
		{
			char *p;
			p = (char *)malloc(100);
			while (scanf("%s", p) != EOF)
			{
				if (p[0] == '/')
					break;
				int color = 0;
				if (strstr(p, "SPADES"))
					color = 0;
				if (strstr(p, "HEARTS"))
					color = 1;
				if (strstr(p, "CLUBS"))
					color = 2;
				if (strstr(p, "DIAMONDS"))
					color = 3;
				scanf("%s", p);
				int point = getInt(p);
				flopMsg(color, point);
			}
			free(p);
		}
		void parseShowDown()
		{
			char *p;
			p = (char *)malloc(100);
			while (scanf("%s", p) != EOF)
				if (p[0] == '/')
					break;
			while (scanf("%s", p) != EOF)
			{
				if (p[0] == '/')
					break;
				int rank, pid;
				sscanf(p, "%d:", &rank);
				char c[2][10], pp[2][10], str[20];
				scanf("%d %s %s %s %s %s", &pid, c[0], pp[0], c[1], pp[1], str);
				int color[2], point[2];
				For(i, 0, 1)
				{
					if (strstr(c[i], "SPADES"))
						color[i] = 0;
					if (strstr(c[i], "HEARTS"))
						color[i] = 1;
					if (strstr(c[i], "CLUBS"))
						color[i] = 2;
					if (strstr(c[i], "DIAMONDS"))
						color[i] = 3;
					point[i] = getInt(pp[i]);
				}
				int nut = 0;
				switch (str[0])
				{
					case 'H':
						{
							nut = 0;
							break;
						}
					case 'O':
						{
							nut = 1;
							break;
						}
					case 'T':
						{
							if (str[1] == 'W')
								nut = 2;
							else
								nut = 3;
							break;
						}
					case 'S':
						{
							if (strstr(str, "-") != NULL)
								nut = 8;
							else
								nut = 4;
							break;
						}
					case 'F':
						{
							if (str[1] == 'L')
								nut = 5;
							if (str[1] == 'U')
								nut = 6;
							if (str[1] == 'O')
								nut = 7;
							break;
						}
				}
				showDownMsg(rank, pid, color[0], point[0], color[1], point[1], nut);
			}
			free(p);
		}
		void parsePotWin()
		{
		}

	public:
		char fileName[50] = {'\0'};
		void bufferInit(int id)
		{
			sprintf(fileName, "%d.buf", id);
		}
		void bufferWrite(char *msg)
		{
			FILE *f = fopen(fileName, "w");
			if (!f)
				syslog(LOG_ERR|LOG_USER, "oops -%m/n");
			fprintf(f, "%s", msg);
			fclose(f);
		}
		void sentAllIn()
		{
			const char* response = "all_in";
			send(m_socket_id, response, (int)strlen(response)+1, 0);
			Me->jetton = 0;
			pInfo.P[pInfo.pMemo[my_id]].jetton = 0;
		}
		void sentCheck()
		{
			const char* response = "check";
			send(m_socket_id, response, (int)strlen(response)+1, 0);
		}
		void sentCall()
		{
			const char* response = "call";
			send(m_socket_id, response, (int)strlen(response)+1, 0);
			pInfo.P[pInfo.pMemo[my_id]].jetton -= state[currentState].maxBet;
			Me->jetton -= state[currentState].maxBet;
		}
		void sentRaise(int num)
		{
			std::string response = "raise ";
			std::stringstream ss;
			ss << num;
			std::string s;
			ss >> s;
			response += s;
			send(m_socket_id, response.c_str(), int(response.size())+1, 0);
			pInfo.P[pInfo.pMemo[my_id]].jetton -= num;
			Me->jetton -= num;
		}
		void sentFold()
		{
			const char* response = "fold";
			send(m_socket_id, response, (int)strlen(response)+1, 0);
		}
		void decision()
		{
			state[currentState].opNum = 0;
			for (int i = 1; i <= state[currentState].totPlayer; i++)
				if (state[currentState].player[i].identity >= 0)
					state[currentState].opNum++;
			int act = 0;
			if (state[currentState].numFlop == 0)
			{
				act = Two_cards::Choose(state[currentState], *Me);
			}
			if (state[currentState].numFlop == 3)
			{
				act = Five_cards::Choose(state[currentState], *Me, rank);
			}
			if (state[currentState].numFlop == 4)
			{
				//act = Five_cards::Choose(state[currentState], *Me, rank);
				act = Six_cards::Choose(state[currentState], *Me, rank);
			}
			if (state[currentState].numFlop == 5)
			{
				//act = Five_cards::Choose(state[currentState], *Me, rank);
				act = Seven_cards::Choose(state[currentState], *Me, rank);
			}
			if (act == 1)
				sentCall();
			if (act == 3)
				sentAllIn();
			if (act == 4)
				sentFold();
			if (act == 5)
				sentCheck();
			if (act > 6)
				sentRaise(act);
		}
		bool server_msg_process()
		{
			//FILE *f = fopen(fileName, "r");
			freopen(fileName, "r", stdin);
			char *msg;
			msg = (char *)malloc(100);
			while (scanf("%s", msg) != EOF)
			{
				if (NULL != strstr(msg, "game-over"))
				{
					//fclose(f);
					fclose(stdin);
					free(msg);
					return false;
				}
				if (NULL != strstr(msg, "inquire/"))
				{
					parseInquire();
					state[currentState].maxBet = std::max(0, state[currentState].maxBet - Me->bet);
					decision();
					//sentAllIn();
					//sentRaise(100);
					//sentFold();
				}
				if (NULL != strstr(msg, "seat/"))
				{
					newGame();
					parseSeat();
					/*
					std::string str = "decision";
					str += char(Me->pid % 10 + '0');
					FILE *f1 = fopen(str.c_str(), "a");
					fprintf(f1, "%d Hands\n", currentState);
					fclose(f1);
					*/
				}
				if (NULL != strstr(msg, "blind/"))
				{
					parseBlind();
				}
				if (NULL != strstr(msg, "hold/"))
				{
					parseHold();
				}
				if (NULL != strstr(msg, "flop/"))
				{
					parseFlop();
					opModel.initRaise();
				}
				if (NULL != strstr(msg, "turn/"))
				{
					parseFlop();
					opModel.initRaise();
				}
				if (NULL != strstr(msg, "river/"))
				{
					parseFlop();
					opModel.initRaise();
					/*
					std::string str = "decision";
					str += char(Me->pid % 10 + '0');
					FILE *f = fopen(str.c_str(), "a");
					for (int i = 1; i <= state[currentState].totPlayer; i++)
						fprintf(f, "%d: %.4f\n", state[currentState].player[i].pid, state[currentState].player[i].prob);
					for (int i = 0; i < 8; i++)
					{
						fprintf(f, "%d: ", i);
						rep(j, 4)
							rep(k, 3)
								fprintf(f, "%.4f ", opModel.P[i].probability[j][k]);
						fprintf(f, "\n");
					}
					fclose(f);
					*/
				}
				if (NULL != strstr(msg, "showdown/"))
				{
					parseShowDown();
				}
				if (NULL != strstr(msg, "pot-win/"))
				{
				}

				if (NULL != strstr(msg, "notify/"))
				{
					parseInquire();
				}
			}
			free(msg);
			//fclose(f);
			fclose(stdin);
			return true;
		}
};
int main(int argc, char* argv[])
{
	if (argc != 6)
	{
		return -1;
	}
	//提取命令行参数
	in_addr_t server_ip = inet_addr(argv[1]);
	in_port_t server_port = atoi(argv[2]);
	in_addr_t my_ip = inet_addr(argv[3]);
	in_port_t my_port = atoi(argv[4]);
	my_id = atoi(argv[5]);
	char* my_name = strrchr(argv[0], '/');
	if (my_name == NULL)
	{
		my_name = argv[0];
	}
	else
	{
		my_name++;
	}
	//创建socket
	m_socket_id = socket(AF_INET, SOCK_STREAM, 0);
	//绑定自己的IP
	sockaddr_in my_addr;
	my_addr.sin_addr.s_addr = my_ip;
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(my_port);
	long flag = 1;
	setsockopt(m_socket_id, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));
	if (bind(m_socket_id, (sockaddr*)&my_addr, sizeof(sockaddr)) < 0)
	{
		printf("bind failed: %m\n");
		return -1;
	}
	//连接服务器
	sockaddr_in server_addr;
	server_addr.sin_addr.s_addr = server_ip;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
    srand((unsigned)time(NULL));
	while (0 != connect(m_socket_id, (sockaddr*)&server_addr, sizeof(sockaddr)))
	{

		printf("%s\n",my_name);

		usleep(1*1000);
	}
	//向server注册
	char reg_msg[50] = "";
	snprintf(reg_msg, sizeof(reg_msg) - 1, "reg: %d %s need_notify\n", my_id, my_name);
	//snprintf(reg_msg, sizeof(reg_msg) - 1, "reg: %d %s\n", my_id, my_name);
	send(m_socket_id, reg_msg, (int)strlen(reg_msg)+1, 0);
	Message M;
	//开始游戏
	M.bufferInit(my_id);
	rank->init();
	Two_cards::Init();
	while (1)
	{
		char buffer[1024] = {'\0'};
		int size = recv(m_socket_id, buffer, sizeof(buffer) - 1, 0);
		if (size > 0)
		{
			//printf("%d %d\n", pInfo.P[pInfo.pMemo[my_id]].money, pInfo.P[pInfo.pMemo[my_id]].jetton);
			M.bufferWrite(buffer);
			if (!M.server_msg_process())
				break;
		}
		if(Me!=NULL)std::cerr<<"My jetton: "<<Me->jetton<<std::endl;
	}
	close(m_socket_id);
	//char msg[50] = "end program";
	//M.bufferWrite(msg);
	return 0;
}
