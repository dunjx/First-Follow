#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<set>

//E je epsilon, F je kraj ulaza.
#define EPS 'E'
#define EOI 'F'

using namespace std;

// Rules cuva pravila, gde je kljuc neterminal iz koga se izvodi pravilo,
// a vrednost je vektor svih pravila u obliku stringova.
map<string, vector<string>> rules;

map<string, set<char>> first;
map<string, set<char>> follow;
int main()
{
	string net, pom;
	vector<string> neterminals;
	
	cin >> pom;
	while(pom.compare("end") != 0)
	{
		net = pom;
		vector<string> rule;
		cin.ignore(3);
		
		do {
			cin >> pom;
			rule.push_back(pom);
			cin >> pom;
			
			if (pom.compare("end") == 0)
				break;
			
		}while(pom.compare("|") == 0);
	
// Unosenje pravila u mapu oblika (neterminal, vektor pravila).
	rules[net] = rule;	
	neterminals.push_back(net);
	if (pom.compare("end") == 0)
		break;	
	}
	
// Unosenje pocetnih terminala u skup FIRST. 
	for (auto it = neterminals.begin(); it != neterminals.end(); it++)
	{
	// Prolazi se svim pravilima za fiksirani neterminal i upisuju se terminali ako njima
	// pocinje pravilo.
	
		for (auto it2 = rules[*it].begin(); it2 != rules[*it].end(); it2++)
		{
			if (!isupper((*it2).at(0)) && *it2 != "eps")
			{
				first[*it].insert((*it2).at(0));
			}
			// Ako postoji epsilon pravilo, upsijuje se EPS makro u pravila. 
			if (*it2 == "eps")
			{
				first[*it].insert(EPS);
			}
		}
	}
	
// Unosenje u skup FIRST sve pocetne terminale pravila koja pocinju neterminalima.
// Kroz proces se prolazi dva puta u slucaju da pravila nisu zadata redom pogodnim za odredjivanje
// first i follow skupova, u drugom prolasku se uzimaju u obzir sve ono sto je prvi put nije bilo obradjeno.
	for (unsigned br_provera=0; br_provera < 2; br_provera++)
	{
		for (auto it = neterminals.begin(); it != neterminals.end(); it++)
		{
		// Za svaki neterminal se pamti da li je imao epsilon pravilo ili ne,
		// za slucaj da se kasnije ubaci, da li ga treba izbaciti ili ne.
			bool EPSindicator = false;
			if (first[*it].find(EPS) != first[*it].end())
			{
				EPSindicator = true;
			}
				
			for (auto it2 = rules[*it].begin(); it2 != rules[*it].end(); it2++)
			{
			// Za svako pravilo se proverava da li pocinje neterminalom.
				if (isupper((*it2).at(0)))
				{
					int i = 0;
					// Ako pravilo pocinje neterminalom, sve iz skupa first tog terminala se
					// stavlja i u skup frist trenutnog analiziranog neterminala.
					string pom_neter = (*it2).substr(i,1);
					first[*it].insert(first[pom_neter].begin(),first[pom_neter].end());
					
					// U petlji se prolazi kroz sve prve neterminale koji imaju epsilon pravila
					// i cije first elemente treba staviti u skup first posmatranog neterminala.
					while ((first[pom_neter]).find(EPS) != (first[pom_neter]).end())
					{
						if ((*it2).at(i) != (*it2).back())
						{
							i++;
							if (isupper((*it2).at(i)))
							{
							// Ako je sledecu element neterminal, svi njegovi first elementi se stavljaju
							// u first skup koji se trenutno obradjuje i nastavlja se petlja.
								pom_neter = (*it2).substr(i,1);
								first[*it].insert(first[pom_neter].begin(),first[pom_neter].end());
							}
							else
							{
							// Ako je sledeci element terminal, ubacuje se u first skup i zavrsava se petlja.
								first[*it].insert((*it2).at(i));
								break;
							}
						}
						else
						{
						// U else se ulazi ako se pravilo sastoji samo od neterminala koji imaju epsilon pravila.
						// Zbog toga se indikator postavlja na true.
							EPSindicator = true;
							break;
						}
					}
					// Ako je ubaceno epsilon koje nije trebalo da bude ubaceno, brise se.
					if (EPSindicator == false)
						first[*it].erase(EPS);
				}
			}
		}
	}
	
// Ispis skupa FIRST.
	cout << "FIRST skupovi: "<< endl;
	for (auto it = neterminals.begin(); it != neterminals.end(); it++)
	{
		cout << *it << ": ";
		
		for (auto it3 = first[*it].begin(); it3 != first[*it].end(); it3++)
		{
			cout << *it3 << " ";
		}
		cout << endl;
	}

	
// Unosenje u skup FOLLOW sve terminale nakon pojavljivanja nerterminala u drugim pravilima.
	// U follow skup pocetnog neterminala se unosi kraj ulaza EOI makro.
	follow[*neterminals.begin()].insert(EOI);
	
	//Prolazi se dva puta kroz pravila ako nisu rasporedjena tako da mogu da se obrade u jednom prolasku.
	for (unsigned br_provera=0; br_provera < 2; br_provera++)
	{
		for(auto it = neterminals.begin(); it != neterminals.end(); it++)
		{
			for (auto neter = rules.begin(); neter != rules.end(); neter++)
			{
				string left_neter = neter->first;
				vector<string> rule = neter->second;
				
				for(auto it2 = rule.begin(); it2 != rule.end(); it2++)
				{
					// Pronalazi se mesto na kom se, u pravilu, nalazi neterminal koji se obradjuje.
					int pos = (*it2).find(*it);
					while (pos != -1 && (int)(*it2).size() != pos+1)
					{
						// Uzima se podniska, od tog terminala do kraja, jer je ona potrebna za dalju proveru.
						string next_ter = (*it2).substr(pos+1);
						//Ako je sledeci terminal, onda se ubacuje u follow skup.
						if (!isupper(next_ter.at(0)))
							follow[*it].insert(next_ter.at(0));
						else
						{
							int i = 0;
							// Ako je sledeci neterminal, elementi njegovog first skupa se ubacuju u trenutno
							// obradjivani follow skup. Ni u jednom follow skupu ne sme da bude epsilon, tako da
							// se ono uvek izbacuje.
							string following_net = next_ter.substr(i,1);
							follow[*it].insert(first[following_net].begin(),first[following_net].end());
							follow[*it].erase(EPS);
							
							//Prolazi se kroz sve neterminale koji slede i imaju epsilon pravilo.
							while (first[following_net].find(EPS) != first[following_net].end())
							{							
								if (next_ter.at(i) != next_ter.back())
								{
									i++;
									if (isupper(next_ter.at(i)))
									{
										// Ako je sledeci neterminal, njegovi elementi first skupa se ubacuju u trenutni
										// follow skup i petlja se nastavlja.
										following_net = next_ter.substr(i,1);
										follow[*it].insert(first[following_net].begin(),first[following_net].end());
										follow[*it].erase(EPS);
									}
									else 
									{
									// Ako je sledeci terminal, onda se ubacuje u follow skup i petlja se prekida.
										follow[*it].insert(next_ter.at(i));
										break;
						 			}
								}
								else
								{
								// U else se ulazi ako se do kraja nalaze samo neterminali koji svi imaju epsilon pravila.
								// Tada se svi elementi skupa follow neterminala iz koga je izvedeno pravilo stavljaju u
								// trenutan skup follow.
									follow[*it].insert(follow[left_neter].begin(),follow[left_neter].end());
									break;
								}
							}	
						}
						// Pozicija se postavlja na sledece pojavljivanje obradjivanog neterminala.
						pos = (*it2).find(*it,pos+1);
					}
					if (pos != -1 && (*it2).at(pos) == (*it2).back())
					{
						// Ako se neterminal nalazi na kraju pravila, elementi skupa follow neterminala iz koga je izvedeno
						// pravilo se ubacuju u trenutno obradjivani skup follow.
						follow[*it].insert(follow[left_neter].begin(),follow[left_neter].end());				
					}
				}
			}
		}
	}
	
	
// Ispis skupa FOLLOW.
	cout << "FOLLOW skupovi: "<< endl;
	for (auto it = neterminals.begin(); it != neterminals.end(); it++)
	{
		cout << *it << ": ";
		
		for (auto it3 = follow[*it].begin(); it3 != follow[*it].end(); it3++)
		{
			cout << *it3 << " ";
		}
		cout << endl;
	}

	return 0;
}
