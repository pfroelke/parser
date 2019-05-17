#include <iostream>
#include <stdlib.h>
#include <fstream>

using namespace std;

int op_limit, op_counter = 0;
char next_element[32];
char buff[100000];
int variables[52] = { 0 };

bool compareArrays(char* a1, char* a2) {
	int i = 0;
	while (a1[i] != '\0' || a2[i] != '\0') {
		if (a1[i] != a2[i]) return 0;
		i++;
	}
	if (a1[i] == a2[i]) return 1;
	return 0;
}

void copyArrays(char* to, char* from = NULL) {
	int i;
	if (from == NULL) {
		for (i = 0; i < 32; i++) {
			to[i] = '\0';
		}
	}
	else {
		for (i = 0; from[i] != '\0'; i++) {
			to[i] = from[i];
		}
		to[i] = from[i];
	}
}

int getVarIndex(char* c) {
	int index = 0;
	if (c[0] >= 'A' && c[0] <= 'Z') {
		index = c[0] - 'A';
	}
	else if (c[0] >= 'a' && c[0] <= 'z') {
		index = c[0] - 'a' + 26;
	}
	return index;
}

class Element {
public:
	virtual ~Element() {};
	virtual int execute() = 0;
	virtual bool isNotNul() = 0;
	virtual Element* getPreviousElement() = 0;
};

class Constant : public Element {
	int value;

public:
	Constant(int w) : value(w) {};

	virtual int execute() {
		return value;
	}
	virtual bool isNotNul() {
		return true;
	}
	virtual Element* getPreviousElement() {
		return this;
	}
};

class Variable : public Element
{
	char variable_name[32];
	int value;
	bool is_not_nul;

public:
	Variable(char* n) {
		copyArrays(variable_name, n);
		is_not_nul = false;
	};

	virtual int execute() {
		return value;
	}

	bool isNotNul() {
		return is_not_nul;
	}

	int setValue(int w, bool n) {
		value = w;
		is_not_nul = n;
		return true;
	}

	char* getVariableName() {
		return variable_name;
	}

	virtual Element* getPreviousElement() {
		return this;
	}
};

Variable*** variableArray;

class Operator : public Element {
	char symbol[3];
	Element *left_el, *right_el;
	bool is_not_nul;

public:
	Operator(char* s, Element* l, Element* r) : left_el(l), right_el(r) {
		copyArrays(symbol, s);
		if (l == NULL) left_el = new Constant(0);
		if (r == NULL) right_el = new Constant(0);
		is_not_nul = true;
	};

	virtual ~Operator() {
		delete left_el;
		delete right_el;
	}

	virtual int execute() {
		op_counter++;
		if (op_counter >= op_limit) { throw "END"; }

		if (compareArrays(symbol, "*")) {
			if (!right_el->isNotNul() || !left_el->isNotNul()) { is_not_nul = false; }
			else { return (left_el->execute() * right_el->execute()); }
		}
		else if (compareArrays(symbol, "+")) {
			if (!right_el->isNotNul() || !left_el->isNotNul()) { is_not_nul = false; }
			else { return (left_el->execute() + right_el->execute()); }
		}
		else if (compareArrays(symbol, "-")) {
			if (!right_el->isNotNul() || !left_el->isNotNul()) { is_not_nul = false; }
			else { return (left_el->execute() - right_el->execute()); }
		}
		else if (compareArrays(symbol, "/")) {
			if (!right_el->isNotNul() || !left_el->isNotNul()) { is_not_nul = false; }
			else {
				int right_el_value = right_el->execute();
				if (right_el_value != 0) {
					return (left_el->execute() / right_el_value);
				}
				else { is_not_nul = false; }
			}
		}
		else if (compareArrays(symbol, "%")) {
			if (!right_el->isNotNul() || !left_el->isNotNul()) { is_not_nul = false; }
			else {
				int right_el_value = right_el->execute();
				if (right_el_value != 0) {
					return (left_el->execute() % right_el_value);
				}
				else { is_not_nul = false; }
			}
		}
		else if (compareArrays(symbol, "=")) {
			Variable* var = dynamic_cast<Variable*> (left_el->getPreviousElement());
			int right_el_value = right_el->execute();
			if (var) var->setValue(right_el_value, right_el->isNotNul());
			left_el->execute();
			return right_el_value;
		}
		else if (compareArrays(symbol, "|")) {
			left_el->execute();
			right_el->execute();
			is_not_nul = (left_el->isNotNul() | right_el->isNotNul());
		}
		else if (compareArrays(symbol, "&")) {
			left_el->execute();
			right_el->execute();
			is_not_nul = (left_el->isNotNul() & right_el->isNotNul());
		}
		else if (compareArrays(symbol, "!=")) {
			is_not_nul = ((left_el->execute() != right_el->execute()) && (left_el->isNotNul() == right_el->isNotNul()));
		}
		else if (compareArrays(symbol, "==")) {
			is_not_nul = ((left_el->execute() == right_el->execute()) && (left_el->isNotNul() == right_el->isNotNul()));
		}
		else if (compareArrays(symbol, "<")) is_not_nul = (left_el->execute() < right_el->execute());
		else if (compareArrays(symbol, ">")) is_not_nul = (left_el->execute() > right_el->execute());
		else if (compareArrays(symbol, "<=")) is_not_nul = (left_el->execute() <= right_el->execute());
		else if (compareArrays(symbol, ">=")) is_not_nul = (left_el->execute() >= right_el->execute());
		else if (compareArrays(symbol, "!")) {
			right_el->execute();
			is_not_nul = !(right_el->isNotNul());
		}
		return 0;
	}
	virtual bool isNotNul() {
		return (is_not_nul);
	}

	virtual Element* getPreviousElement() {
		return right_el->getPreviousElement();
	}
};

class ControlStatement {
public:
	virtual void execute() = 0;
};

class Block : public ControlStatement
{
	ControlStatement* left_statement, *right_statement;

public:
	Block(ControlStatement* l, ControlStatement* r) : left_statement(l), right_statement(r) {};

	virtual void execute() {
		if (op_counter >= op_limit) { throw "END"; }
		left_statement->execute();
		right_statement->execute();
	}
};

class IfClause : public ControlStatement
{
	Element* boolean_expression;
	ControlStatement* statements;

public:
	IfClause(Element* w, ControlStatement* p) : boolean_expression(w), statements(p) {};

	virtual void execute() {
		if (op_counter >= op_limit) { throw "END"; }
		boolean_expression->execute();
		if (op_counter++ && statements != NULL && boolean_expression->isNotNul()) statements->execute();
	}
};

class WhileLoop : public ControlStatement
{
	Element* boolean_expression;
	ControlStatement* statements;

public:
	WhileLoop(Element* w, ControlStatement* p) : boolean_expression(w), statements(p) {};
	
	virtual void execute() {
		if (op_counter >= op_limit) { throw "END"; }
		boolean_expression->execute();
		while (op_counter++ && statements != NULL && boolean_expression->isNotNul()) {
			statements->execute();
			boolean_expression->execute();
		}
	}
};

class SingleControlStatement : public ControlStatement
{
	Element* to_execute;

public:
	SingleControlStatement(Element* w) : to_execute(w) {};

	virtual void execute() {
		to_execute->execute();
	}
};

class Parser
{
	char* input;
	int position;

public:
	Parser(char* w) : input(w), position(0) {};

	void skipWhitespaces() {
		while (input[position] == ' ' || input[position] == '\t' || input[position] == '\n')
			position++;
	};

	void getNextElement(char* s)
	{
		skipWhitespaces();
		int i = 0;
		if (isalpha(input[position])) {
			while (isalpha(input[position]))
			{
				s[i] = input[position];
				position++;
				i++;
			}
		}
		else if (isdigit(input[position])) {
			while (isdigit(input[position]))
			{
				s[i] = input[position];
				position++;
				i++;
				skipWhitespaces();
			}
		}
		else {
			switch (input[position]) {
			case '!':
			case '<':
			case '>':
			case '=':
				if (input[position + 1] == '=') {
					s[i] = input[position];
					s[i + 1] = input[position + 1];
					position = position + 2;
					i = i + 2;
					break;
				}
				s[i] = input[position];
				position++;
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
				if (input[position + 1] == 'u') {
					s[i] = input[position];
					s[i + 1] = input[position + 1];
					position = position + 2;
					i = i + 2;
					break;
				}
			case '/':
			case '*':
			case '%':
				s[i] = input[position];
				position++;
				i++;
				break;
			default:
				while (!isdigit(input[position]) && !isalpha(input[position]) && input[position] != '\0')
				{
					s[i] = input[position];
					position++;
					i++;
					skipWhitespaces();
				}
				break;
			}
		}
		s[i] = '\0';

	}

	ControlStatement* analyseCode()
	{
		ControlStatement* p = analyseBlock();
		if (true)
			return p;
		else
			return NULL;
	}

	ControlStatement* analyseBlock() {
		getNextElement(next_element);
		ControlStatement* p = analyseStatements();
		while (next_element[0] != '}' && next_element[0] != EOF) {
			ControlStatement* q = analyseStatements();
			if (q) p = new Block(p, q);
		}

		return p;
	}

	ControlStatement* analyseStatements()
	{
		if (compareArrays(next_element, "{"))
		{
			ControlStatement* p = analyseBlock();
			if (compareArrays(next_element, "}"))
			{
				getNextElement(next_element);
				return p;
			}
			else
				return p;
		}
		else if (compareArrays(next_element, "?")) {
			getNextElement(next_element);
			return analyseIfClause();
		}
		else if (compareArrays(next_element, "@")) {
			getNextElement(next_element);
			return analyseWhileLoop();
		}
		else {
			return analyseSingleStatement();
		}
	}

	ControlStatement* analyseIfClause()
	{
		Element* e = analyseAssignment();
		ControlStatement* p = analyseStatements();
		char s[32];
		return new IfClause(e, p);
	}

	ControlStatement* analyseWhileLoop()
	{
		Element* e = analyseAssignment();
		ControlStatement* p = analyseStatements();
		return new WhileLoop(e, p);
	}

	ControlStatement* analyseSingleStatement()
	{
		return new SingleControlStatement(analyseAssignment());
	}

	Element* analyseAssignment() {
		Element* i = analyseOr();

		while (compareArrays(next_element, "=")) {
			char symbol[2];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			Element* e2 = analyseOr();
			i = new Operator(symbol, i, e2);
		}
		return i;
	};

	Element* analyseOr() {
		Element* i = analyseAnd();

		while (next_element[0] == '|') {
			char symbol[2];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = new Operator(symbol, i, analyseAnd());
		}
		return i;
	};

	Element* analyseAnd() {
		Element* i = analyseEquals();

		while (next_element[0] == '&') {
			char symbol[2];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = new Operator(symbol, i, analyseEquals());
		}
		return i;
	};

	Element* analyseEquals() {
		Element* i = analyseMoreLess();

		while (compareArrays(next_element, "!=") || compareArrays(next_element, "==")) {
			char symbol[3];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = new Operator(symbol, i, analyseMoreLess());
		}
		return i;
	};

	Element* analyseMoreLess() {
		Element* i = analyseSumDifference();

		while (next_element[0] == '<' || next_element[0] == '>' || compareArrays(next_element, "<=") || compareArrays(next_element, ">=")) {
			char symbol[3];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = new Operator(symbol, i, analyseSumDifference());
		}
		return i;
	};

	Element* analyseSumDifference() {
		Element* i = analyseProductQuotient();

		while (next_element[0] == '+' || next_element[0] == '-') {
			char symbol[2];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = new Operator(symbol, i, analyseProductQuotient());
		}
		return i;
	};

	Element* analyseProductQuotient() {
		Element* i = analyseNegation();

		while (next_element[0] == '*' || next_element[0] == '/' || next_element[0] == '%') {
			char symbol[2];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = new Operator(symbol, i, analyseNegation());
		}
		return i;
	};

	Element* analyseNegation() {
		Element* i = analyseComponent();

		while (compareArrays(next_element, "!")) {
			char symbol[2];
			copyArrays(symbol, next_element);
			getNextElement(next_element);
			i = analyseNegation();
			i = new Operator(symbol, NULL, i);
		}
		return i;
	};

	Element* analyseComponent() {
		if (next_element[0] >= '0' && next_element[0] <= '9') {
			return analyseConstant(false);
		}
		else if (next_element[0] >= 'A' && next_element[0] <= 'Z' || next_element[0] >= 'a' && next_element[0] <= 'z') {
			return analyseVariable();
		}
		else if (next_element[0] == '(') {
			return analyseBrackets();
		}
		else if (next_element[0] == '-') {
			return analyseMinus();
		}
		else return NULL;
	};

	Element* analyseMinus() {
		char symbol[2];
		copyArrays(symbol, next_element);
		getNextElement(next_element);
		Element* e;
		if (next_element[0] >= '0' && next_element[0] <= '9') {
			e = analyseConstant(true);
		}
		else if (next_element[0] >= 'A' && next_element[0] <= 'Z' || next_element[0] >= 'a' && next_element[0] <= 'z') {
			e = new Operator(symbol, NULL, analyseVariable());
		}
		else if (next_element[0] == '(') {
			e = new Operator(symbol, NULL, analyseBrackets());
		}
		return e;
	};

	Element* analyseConstant(bool neg) {
		int w = 0;
		int i = 0;
		for (; next_element[i] >= '0' && next_element[i] <= '9'; i++) {
			w *= 10;
			w += next_element[i] - '0';
		}
		getNextElement(next_element);
		if (neg) return new Constant(-w);
		return new Constant(w);
	};

	Element* analyseVariable() {
		int index = getVarIndex(next_element);
		for (int i = 0; i < variables[index]; i++) {
			if (compareArrays(variableArray[index][i]->getVariableName(), next_element)) {
				getNextElement(next_element);
				return variableArray[index][i];
			}
		}
		variableArray[index][variables[index]] = new Variable(next_element);
		getNextElement(next_element);
		variables[index]++;
		return variableArray[index][variables[index]-1];
	};

	Element* analyseBrackets() {
		getNextElement(next_element);
		Element* w = analyseAssignment();

		if (next_element[0] == ')') {
			getNextElement(next_element);
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
	// Locals
	
	char c;
	int i = 0, tmp, variables_amount = 0, variables_to_print_amount = 0;
	char variable_name[32] = "";
	Variable* variables_to_print[500];
	variableArray = new Variable**[52];
	for (int i = 0; i < 52; i++) {
		variableArray[i] = new Variable*[500];
	}
	

	// Opening file with input
	FILE *fp;
	fp = fopen("input.txt", "r");

	// Scaning the operation limit
	fscanf_s(fp, "%s", &buff);
	sscanf(buff, "%d", &op_limit);
	fgetc(fp); // Skipping new line ('\n')

	// Scaning variables to print
	while ((c = fgetc(fp)) != '\n' && c != EOF) {
		if (c == ' ') {
			tmp = getVarIndex(variable_name);
			variableArray[tmp][variables[tmp]] = new Variable(variable_name);
			variables_to_print[variables_amount] = variableArray[tmp][variables[tmp]];
			variables[tmp]++;
			variables_amount++;
			variables_to_print_amount++;
			copyArrays(variable_name);
			i = 0;
		}
		else if (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') {
			variable_name[i] = c;
			i++;
		}
	}
	tmp = getVarIndex(variable_name);
	variableArray[tmp][variables[tmp]] = new Variable(variable_name);
	variables_to_print[variables_amount] = variableArray[tmp][variables[tmp]];
	variables[tmp]++;
	variables_amount++;
	variables_to_print_amount++;
	copyArrays(variable_name);
	i = 0; // Iterators reset

	// Scaning script code into buffer
	while (!feof(fp)) {
		c = fgetc(fp);
		buff[i++] = c;
	}
	buff[i] = '\0';

	// If we scaned something
	if (buff[0] != '\0') {
		// Create parser
		Parser parser(buff);
		// Parse 
		ControlStatement* p = parser.analyseCode();
		// Execute parsed code
		try {
			p->execute();
		}
		catch (char const*) {

		}
	}

	// Output
	cout << op_counter << endl;
	for (int i = 0; i < variables_to_print_amount; i++) {
		copyArrays(variable_name, variables_to_print[i]->getVariableName());
		if ((variable_name[0] >= 'A' && variable_name[0] <= 'Z') || (variable_name[0] >= 'a' && variable_name[0] <= 'z')) {
			if (!variables_to_print[i]->isNotNul()) {
				cout << variable_name << " Nul" << endl;
			}
			else {
				cout << variable_name << " " << variables_to_print[i]->execute() << endl;
			}
		}
			copyArrays(variable_name);
	}
	return 0;
}