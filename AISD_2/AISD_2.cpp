#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;
int operacje, wykonane = 0;
char nastepny_element[32];
char buff[100000];
int zmienne[52] = { 0 };

bool porownaj_tablice(char* a1, char* a2) {
	int i = 0;
	while (a1[i] != '\0' || a2[i] != '\0') {
		if (a1[i] != a2[i]) return 0;
		i++;
	}
	if (a1[i] == a2[i]) return 1;
	return 0;
}

void kopiuj_tablice(char* do_ktorej, char* z_ktorej = NULL) {
	int i;
	if (z_ktorej == NULL) {
		for (i = 0; i < 32; i++) {
			do_ktorej[i] = '\0';
		}
	}
	else {
		for (i = 0; z_ktorej[i] != '\0'; i++) {
			do_ktorej[i] = z_ktorej[i];
		}
		do_ktorej[i] = z_ktorej[i];
	}
}

int index_zmiennej(char* n) {
	int index = 0;
	if (n[0] >= 'A' && n[0] <= 'Z') {
		index = n[0] - 'A';
	}
	else if (n[0] >= 'a' && n[0] <= 'z') {
		index = n[0] - 'a' + 26;
	}
	return index;
}

class Element {
public:
	virtual ~Element() {};
	virtual int wykonaj() = 0;
	virtual bool stan_nul() = 0;
	virtual Element* ostatni_element() = 0;
};

class Stala : public Element {
	int wartosc;

public:
	Stala(int w) : wartosc(w) {};

	virtual int wykonaj() {
		return wartosc;
	}
	virtual bool stan_nul() {
		return true;
	}
	virtual Element* ostatni_element() {
		return this;
	}
};

class Zmienna : public Element
{
	char nazwaZmiennej[32];
	int wartosc;
	bool stanNul;

public:
	Zmienna(char* n) {
		kopiuj_tablice(nazwaZmiennej, n);
		stanNul = false;
	};

	virtual int wykonaj() {
		//if (!stanNul) return false;
		return wartosc;
	}

	bool stan_nul() {
		return stanNul;
	}

	int ustaw_wartosc(int w, bool n) {
		wartosc = w;
		stanNul = n;
		return true;
	}

	int getWartosc() {
		return wartosc;
	}

	char* getNazwa() {
		return nazwaZmiennej;
	}

	virtual Element* ostatni_element() {
		return this;
	}
};

Zmienna*** varTabTEST;

class Operator : public Element {
	char symbol[3];
	Element *lewa, *prawa;
	bool stanNul;

public:
	Operator(char* s, Element* l, Element* r) : lewa(l), prawa(r) {
		kopiuj_tablice(symbol, s);
		if (l == NULL) lewa = new Stala(0);
		if (r == NULL) prawa = new Stala(0);
		stanNul = true;
	};

	virtual ~Operator() {
		delete lewa;
		delete prawa;
	}

	virtual int wykonaj() {
		wykonane++;
		if (wykonane >= operacje) { throw "KONIEC"; }

		if (porownaj_tablice(symbol, "*")) {
			if (!prawa->stan_nul() || !lewa->stan_nul()) { stanNul = false; }
			else { return (lewa->wykonaj() * prawa->wykonaj()); }
		}
		else if (porownaj_tablice(symbol, "+")) {
			if (!prawa->stan_nul() || !lewa->stan_nul()) { stanNul = false; }
			else { return (lewa->wykonaj() + prawa->wykonaj()); }
		}
		else if (porownaj_tablice(symbol, "-")) {
			if (!prawa->stan_nul() || !lewa->stan_nul()) { stanNul = false; }
			else { return (lewa->wykonaj() - prawa->wykonaj()); }
		}
		else if (porownaj_tablice(symbol, "/")) {
			if (!prawa->stan_nul() || !lewa->stan_nul()) { stanNul = false; }
			else {
				int prawaWartosc = prawa->wykonaj();
				if (prawaWartosc != 0) {
					return (lewa->wykonaj() / prawaWartosc);
				}
				else { stanNul = false; }
			}
		}
		else if (porownaj_tablice(symbol, "%")) {
			if (!prawa->stan_nul() || !lewa->stan_nul()) { stanNul = false; }
			else {
				int prawaWartosc = prawa->wykonaj();
				if (prawaWartosc != 0) {
					return (lewa->wykonaj() % prawaWartosc);
				}
				else { stanNul = false; }
			}
		}
		else if (porownaj_tablice(symbol, "=")) {
			Zmienna* var = dynamic_cast<Zmienna*> (lewa->ostatni_element());
			int prawaWartosc = prawa->wykonaj();
			if (var) var->ustaw_wartosc(prawaWartosc, prawa->stan_nul());
			lewa->wykonaj();
			return prawaWartosc;
		}
		else if (porownaj_tablice(symbol, "|")) {
			lewa->wykonaj();
			prawa->wykonaj();
			stanNul = (lewa->stan_nul() | prawa->stan_nul());
		}
		else if (porownaj_tablice(symbol, "&")) {
			lewa->wykonaj();
			prawa->wykonaj();
			stanNul = (lewa->stan_nul() & prawa->stan_nul());
		}
		else if (porownaj_tablice(symbol, "!=")) {
			stanNul = ((lewa->wykonaj() != prawa->wykonaj()) && (lewa->stan_nul() == prawa->stan_nul()));
		}
		else if (porownaj_tablice(symbol, "==")) {
			stanNul = ((lewa->wykonaj() == prawa->wykonaj()) && (lewa->stan_nul() == prawa->stan_nul()));
		}
		else if (porownaj_tablice(symbol, "<")) stanNul = (lewa->wykonaj() < prawa->wykonaj());
		else if (porownaj_tablice(symbol, ">")) stanNul = (lewa->wykonaj() > prawa->wykonaj());
		else if (porownaj_tablice(symbol, "<=")) stanNul = (lewa->wykonaj() <= prawa->wykonaj());
		else if (porownaj_tablice(symbol, ">=")) stanNul = (lewa->wykonaj() >= prawa->wykonaj());
		else if (porownaj_tablice(symbol, "!")) {
			prawa->wykonaj();
			stanNul = !(prawa->stan_nul());
		}
		return 0;
	}
	virtual bool stan_nul() {
		return (stanNul);
	}

	virtual Element* ostatni_element() {
		return prawa->ostatni_element();
	}
};

class Polecenie {
public:
	virtual void wykonaj() = 0;
};

class Zloz : public Polecenie
{
	Polecenie* lewe, *prawe;

public:
	Zloz(Polecenie* l, Polecenie* r) : lewe(l), prawe(r) {};

	virtual void wykonaj() {
		if (wykonane >= operacje) { throw "KONIEC"; }
		lewe->wykonaj();
		prawe->wykonaj();
	}
};

class Warunek : public Polecenie
{
	Element* warunek;
	Polecenie* polecenie;

public:
	Warunek(Element* w, Polecenie* p) : warunek(w), polecenie(p) {};

	virtual void wykonaj() {
		if (wykonane >= operacje) { throw "KONIEC"; }
		warunek->wykonaj();
		if (wykonane++ && polecenie != NULL && warunek->stan_nul()) polecenie->wykonaj();
	}
};

class Petla_While : public Polecenie
{
	Element* warunek;
	Polecenie* polecenia;

public:
	Petla_While(Element* w, Polecenie* p) : warunek(w), polecenia(p) {};
	
	virtual void wykonaj() {
		if (wykonane >= operacje) { throw "KONIEC"; }
		warunek->wykonaj();
		while (wykonane++ && polecenia != NULL && warunek->stan_nul()) {
			polecenia->wykonaj();
			warunek->wykonaj();
		}
	}
};

class Polecenie_pojedyncze : public Polecenie
{
	Element* warunek;

public:
	Polecenie_pojedyncze(Element* w) : warunek(w) {};

	virtual void wykonaj() {
		warunek->wykonaj();
	}
};

class Parser
{
	char* wejscie;
	int pozycja;

public:
	Parser(char* w) : wejscie(w), pozycja(0) {};

	void pomin_biale_znaki() {
		while (wejscie[pozycja] == ' ' || wejscie[pozycja] == '\t' || wejscie[pozycja] == '\n')
			pozycja++;
	};

	void Parser::pobierz_zmienna(char* s)
	{
		int i = 0;
		while (isalpha(wejscie[pozycja]))
		{
			s[i] = wejscie[pozycja];
			pozycja++;
			i++;
		}
		s[i] = '\0';
	}

	void Parser::pobierz_nastepny_element(char* s)
	{
		pomin_biale_znaki();
		int i = 0;
		if (isalpha(wejscie[pozycja])) {
			while (isalpha(wejscie[pozycja]))
			{
				s[i] = wejscie[pozycja];
				pozycja++;
				i++;
			}
		}
		else if (isdigit(wejscie[pozycja])) {
			while (isdigit(wejscie[pozycja]))
			{
				s[i] = wejscie[pozycja];
				pozycja++;
				i++;
				pomin_biale_znaki();
			}
		}
		else {
			switch (wejscie[pozycja]) {
			case '!':
			case '<':
			case '>':
			case '=':
				if (wejscie[pozycja + 1] == '=') {
					s[i] = wejscie[pozycja];
					s[i + 1] = wejscie[pozycja + 1];
					pozycja = pozycja + 2;
					i = i + 2;
					break;
				}
				s[i] = wejscie[pozycja];
				pozycja++;
				i++;
				break;
			case '}':
			case '{':
			case '@':
			case '?':
			case '(':
			case ')':
			case '|':
			case '&':
			case '+':
			case '-':
				if (wejscie[pozycja + 1] == 'u') {
					s[i] = wejscie[pozycja];
					s[i + 1] = wejscie[pozycja + 1];
					pozycja = pozycja + 2;
					i = i + 2;
					break;
				}
			case '/':
			case '*':
			case '%':
				s[i] = wejscie[pozycja];
				pozycja++;
				i++;
				break;
			default:
				while (!isdigit(wejscie[pozycja]) && !isalpha(wejscie[pozycja]) && wejscie[pozycja] != '\0')
				{
					s[i] = wejscie[pozycja];
					pozycja++;
					i++;
					pomin_biale_znaki();
				}
				break;
			}
		}
		s[i] = '\0';

	}

	Polecenie* Parser::analizuj_kod()
	{
		Polecenie* p = analizuj_blok();
		if (true)
			return p;
		else
			return NULL;
	}

	Polecenie* Parser::analizuj_blok() {
		pobierz_nastepny_element(nastepny_element);
		Polecenie* p = analizuj_polecenia();
		while (nastepny_element[0] != '}' && nastepny_element[0] != EOF) {
			Polecenie* q = analizuj_polecenia();
			if (q) p = new Zloz(p, q);
		}

		return p;
	}

	Polecenie* Parser::analizuj_polecenia()
	{
		if (porownaj_tablice(nastepny_element, "{"))
		{
			Polecenie* p = analizuj_blok();
			if (porownaj_tablice(nastepny_element, "}"))
			{
				pobierz_nastepny_element(nastepny_element);
				return p;
			}
			else
				return p;
		}
		else if (porownaj_tablice(nastepny_element, "?")) {
			pobierz_nastepny_element(nastepny_element);
			return analizuj_warunek();
		}
		else if (porownaj_tablice(nastepny_element, "@")) {
			pobierz_nastepny_element(nastepny_element);
			return analizuj_pentle_while();
		}
		else {
			return analizuj_polecenie();
		}
	}

	Polecenie* Parser::analizuj_warunek()
	{
		Element* e = analizuj_przypisanie();
		Polecenie* p = analizuj_polecenia();
		char s[32];
		return new Warunek(e, p);
	}

	Polecenie* Parser::analizuj_pentle_while()
	{
		Element* e = analizuj_przypisanie();
		Polecenie* p = analizuj_polecenia();
		return new Petla_While(e, p);
	}

	Polecenie* Parser::analizuj_polecenie()
	{
		return new Polecenie_pojedyncze(analizuj_przypisanie());
	}

	Element* analizuj_przypisanie() {
		Element* i = analizuj_lub();

		while (porownaj_tablice(nastepny_element, "=")) {
			char symbol[2];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			Element* e2 = analizuj_lub();
			i = new Operator(symbol, i, e2);
		}
		return i;
	};

	Element* analizuj_lub() {
		Element* i = analizuj_i();

		while (nastepny_element[0] == '|') {
			char symbol[2];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = new Operator(symbol, i, analizuj_i());
		}
		return i;
	};

	Element* analizuj_i() {
		Element* i = analizuj_rownosc();

		while (nastepny_element[0] == '&') {
			char symbol[2];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = new Operator(symbol, i, analizuj_rownosc());
		}
		return i;
	};

	Element* analizuj_rownosc() {
		Element* i = analizuj_nierownosc();

		while (porownaj_tablice(nastepny_element, "!=") || porownaj_tablice(nastepny_element, "==")) {
			char symbol[3];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = new Operator(symbol, i, analizuj_nierownosc());
		}
		return i;
	};

	Element* analizuj_nierownosc() {
		Element* i = analizuj_sume();

		while (nastepny_element[0] == '<' || nastepny_element[0] == '>' || porownaj_tablice(nastepny_element, "<=") || porownaj_tablice(nastepny_element, ">=")) {
			char symbol[3];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = new Operator(symbol, i, analizuj_sume());
		}
		return i;
	};

	Element* analizuj_sume() {
		Element* i = analizuj_iloczyn();

		while (nastepny_element[0] == '+' || nastepny_element[0] == '-') {
			char symbol[2];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = new Operator(symbol, i, analizuj_iloczyn());
		}
		return i;
	};

	Element* analizuj_iloczyn() {
		Element* i = analizuj_negacje();

		while (nastepny_element[0] == '*' || nastepny_element[0] == '/' || nastepny_element[0] == '%') {
			char symbol[2];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = new Operator(symbol, i, analizuj_negacje());
		}
		return i;
	};

	Element* analizuj_negacje() {
		Element* i = analizuj_skladnik();

		while (porownaj_tablice(nastepny_element, "!")) {
			char symbol[2];
			kopiuj_tablice(symbol, nastepny_element);
			pobierz_nastepny_element(nastepny_element);
			i = analizuj_negacje();
			i = new Operator(symbol, NULL, i);
		}
		return i;
	};

	Element* analizuj_skladnik() {
		if (nastepny_element[0] >= '0' && nastepny_element[0] <= '9') {
			return analizuj_stala(false);
		}
		else if (nastepny_element[0] >= 'A' && nastepny_element[0] <= 'Z' || nastepny_element[0] >= 'a' && nastepny_element[0] <= 'z') {
			return analizuj_zmienna();
		}
		else if (nastepny_element[0] == '(') {
			return analizuj_nawias();
		}
		else if (nastepny_element[0] == '-') {
			return analizuj_minus();
		}
		else return NULL;
	};

	Element* analizuj_minus() {
		char symbol[2];
		kopiuj_tablice(symbol, nastepny_element);
		pobierz_nastepny_element(nastepny_element);
		Element* e;
		if (nastepny_element[0] >= '0' && nastepny_element[0] <= '9') {
			e = analizuj_stala(true);
		}
		else if (nastepny_element[0] >= 'A' && nastepny_element[0] <= 'Z' || nastepny_element[0] >= 'a' && nastepny_element[0] <= 'z') {
			e = new Operator(symbol, NULL, analizuj_zmienna());
		}
		else if (nastepny_element[0] == '(') {
			e = new Operator(symbol, NULL, analizuj_nawias());
		}
		return e;
	};

	Element* analizuj_stala(bool neg) {
		int w = 0;
		int i = 0;
		for (; nastepny_element[i] >= '0' && nastepny_element[i] <= '9'; i++) {
			w *= 10;
			w += nastepny_element[i] - '0';
		}
		pobierz_nastepny_element(nastepny_element);
		if (neg) return new Stala(-w);
		return new Stala(w);
	};

	Element* analizuj_zmienna() {
		int index = index_zmiennej(nastepny_element);
		for (int i = 0; i < zmienne[index]; i++) {
			if (porownaj_tablice(varTabTEST[index][i]->getNazwa(), nastepny_element)) {
				pobierz_nastepny_element(nastepny_element);
				return varTabTEST[index][i];
			}
		}
		varTabTEST[index][zmienne[index]] = new Zmienna(nastepny_element);
		pobierz_nastepny_element(nastepny_element);
		zmienne[index]++;
		return varTabTEST[index][zmienne[index]-1];
	};

	Element* analizuj_nawias() {
		pobierz_nastepny_element(nastepny_element);
		Element* w = analizuj_przypisanie();

		if (nastepny_element[0] == ')') {
			pobierz_nastepny_element(nastepny_element);
			return w;
		}
		else {
			delete w;
			return NULL;
		}
	};
};



int main()
{
	//Zmienne
	
	char c;
	int i = 0, tmp, ilosc_zmiennych = 0, ilosc_zmiennych_do_wypisania = 0;
	char nazwa_zmiennej[32] = "";
	char* tmpTab;
	Zmienna* zmienne_do_wypisania[500];
	varTabTEST = new Zmienna**[52];
	for (int i = 0; i < 52; i++) {
		varTabTEST[i] = new Zmienna*[500];
	}
	

	////Otwarcie pliku
	FILE *fp;
	fp = fopen("input.txt", "r");

	//Wczytywanie licznika operacji
	fscanf(fp, "%s", &buff);
	sscanf(buff, "%d", &operacje);
	fgetc(fp); //Pominiecie nowej linii ('\n')

	//Wczytywanie zmiennych do wypisania
	while ((c = fgetc(fp)) != '\n' && c != EOF) {
		if (c == ' ') {
			tmp = index_zmiennej(nazwa_zmiennej);
			varTabTEST[tmp][zmienne[tmp]] = new Zmienna(nazwa_zmiennej);
			zmienne_do_wypisania[ilosc_zmiennych] = varTabTEST[tmp][zmienne[tmp]];
			zmienne[tmp]++;
			ilosc_zmiennych++;
			ilosc_zmiennych_do_wypisania++;
			kopiuj_tablice(nazwa_zmiennej);
			i = 0;
		}
		else if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') {
			nazwa_zmiennej[i] = c;
			i++;
		}
	}
	tmp = index_zmiennej(nazwa_zmiennej);
	varTabTEST[tmp][zmienne[tmp]] = new Zmienna(nazwa_zmiennej);
	zmienne_do_wypisania[ilosc_zmiennych] = varTabTEST[tmp][zmienne[tmp]];
	zmienne[tmp]++;
	ilosc_zmiennych++;
	ilosc_zmiennych_do_wypisania++;
	kopiuj_tablice(nazwa_zmiennej);
	i = 0; //Reset iteratora

	//Wczytywanie kodu programu do buffora "buff"
	while (!feof(fp)) {
		c = fgetc(fp);
		buff[i++] = c;
	}
	buff[i] = '\0';

	//jezeli buff nie jest pusty
	if (buff[0] != '\0') {
		//Tworzenie Parsera
		Parser parser(buff);
		//Parsowanie kodu na polecenie wykonywalne
		Polecenie* p = parser.analizuj_kod();
		//Wykonywanie polecenia
		try {
			p->wykonaj();
		}
		catch (char const*) {

		}
	}

	//Wypisywanie wyniku
	cout << wykonane << endl;
	for (int i = 0; i < ilosc_zmiennych_do_wypisania; i++) {
		kopiuj_tablice(nazwa_zmiennej, zmienne_do_wypisania[i]->getNazwa());
		if ((nazwa_zmiennej[0] >= 'A' && nazwa_zmiennej[0] <= 'Z') || (nazwa_zmiennej[0] >= 'a' && nazwa_zmiennej[0] <= 'z')) {
			if (!zmienne_do_wypisania[i]->stan_nul()) {
				cout << nazwa_zmiennej << " Nul" << endl;
			}
			else {
				cout << nazwa_zmiennej << " " << zmienne_do_wypisania[i]->wykonaj() << endl;
			}
		}
			kopiuj_tablice(nazwa_zmiennej);
	}
	return 0;
}