#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using std::make_shared;
using std::ostream;
using std::runtime_error;
using std::shared_ptr;
using std::string;

using namespace std;

class LambdaExpr;
using Lambda = shared_ptr<LambdaExpr>;

class LambdaExpr {
public:
    string var;   // Name of the variable or empty if it's not a variable.
    string arg;   // Name of the formal parameter for an abstraction, empty otherwise.
    Lambda body;  // Function body of an abstraction, nullptr otherwise.
    Lambda left;  // Callee of an application, nullptr otherwise.
    Lambda right; // Argument of an application, nullptr otherwise.
};

// Parse functions.
Lambda parse_expr(string s, int &i);
Lambda parse(string s);
Lambda parse_part(const string s, int &i);
Lambda parse_expr(const string str, int &i);
Lambda parse(const string str);

string lambda_type(const Lambda e);

bool find_reference(const Lambda e, const string name);
Lambda rename_references(const Lambda e, const string old_arg, const string new_arg);
Lambda substitute(const Lambda e, const string arg, const Lambda value);

Lambda alpha(const Lambda e, const string new_arg);
Lambda beta(const Lambda e);
Lambda new_lambda();

// Utility functions.
[[noreturn]]
inline void syntax_error()
{
    throw runtime_error("syntax error");
}

[[noreturn]]
inline void alpha_error()
{
    throw runtime_error("can not make alpha-conversion");
}

[[noreturn]]
inline void beta_error()
{
    throw runtime_error("can not apply beta-reduction");
}

string remove_spaces(const string &s);
ostream &operator<<(ostream &out, const Lambda e);
bool operator==(const Lambda l, const Lambda r);
