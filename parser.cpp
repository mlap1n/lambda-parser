#include "parser.h"

Lambda new_lambda() {
    Lambda res = make_shared<LambdaExpr>();
    res->var = "";
    res->arg = "";
    res->body = nullptr;
    res->left = nullptr;
    res->right = nullptr;
    return res;
}

Lambda parse_part(const string s, int &i) {
    if (s[i] == '(') {
        i++;
        Lambda e = parse_expr(s, i);
        if (s[i] == ')') {
            i++;
            return e;
        }
        else {
            syntax_error();
        }
    }
    else if (s[i] == '\\') {
        i++;
        Lambda e = new_lambda();
        e->arg = s[i];
        i++;
        if (s[i] == '.') {
            i++;
            e->body = parse_expr(s, i);
            return e;
        }
        else {
            syntax_error();
        }
    }
    else {
        Lambda e = new_lambda();
        e->var = s[i];
        i++; // skip over variable name
        return e;
    }
}

Lambda parse_expr(const string str, int &i) {
    Lambda left = parse_part(str, i);
    while (i < str.size() && str[i] != ')') {
        Lambda right = parse_part(str, i);
        Lambda e = new_lambda();
        e->left = left;
        e->right = right;
        left = e;
    }
    return left;
}

Lambda parse(const string str) {
    string new_str = remove_spaces(str);
    int i = 0;
    Lambda e = parse_expr(new_str, i);
    if (i == new_str.size()) {
        return e;
    }
    else {
        syntax_error();
    }
}

string lambda_type(const Lambda e) {
    if (e->var != "") {
        return "var";
    }
    else if (e->arg != "" && e->body) {
        return "assoc";
    }
    else if (e->left && e->right) {
        return "app";
    }
    else {
        return "unknown";
    }
}

bool find_reference(const Lambda e, const string name) {
    string type = lambda_type(e);
    if (type == "var") {
        return e->var == name;
    }
    else if (type == "app") {
        bool found_left = find_reference(e->left, name);
        bool found_right = find_reference(e->right, name);
        return found_left || found_right;
    }
    else if (type == "assoc") {
        if (e->arg == name) {
            return false;
        }
        else {
            return find_reference(e->body, name);
        }
    }
    else {
        return false;
    }
}

Lambda rename_references(const Lambda e, const string old_arg, const string new_arg) {
    string type = lambda_type(e);
    if (type == "var") {
        if (e->var == old_arg) {
            Lambda e2 = new_lambda();
            e2->var = new_arg;
            return e2;
        }
        else if (e->var == new_arg) {
            alpha_error();
        }
        else {
            return e;
        }
    }
    else if (type == "app") {
        Lambda new_left = rename_references(e->left, old_arg, new_arg);
        Lambda new_right = rename_references(e->right, old_arg, new_arg);
        if (new_left != e->left || new_right != e->right) {
            Lambda e2 = new_lambda();
            e2->left = new_left;
            e2->right = new_right;
            return e2;
        }
        else {
            return e;
        }
    }
    else if (type == "assoc") {
        if (e->arg == old_arg) {
            if (find_reference(e->body, new_arg)) {
                alpha_error();
            }
            return e;
        }
        else if (e->arg == new_arg) {
            if (find_reference(e->body, old_arg)) {
                alpha_error();
            }
            return e;
        }
        else {
            Lambda new_body = rename_references(e->body, old_arg, new_arg);
            if (new_body != e->body) {
                Lambda e2 = new_lambda();
                e2->arg = e->arg;
                e2->body = new_body;
                return e2;
            }
            else {
                return e;
            }
        }
}
    else {
        alpha_error();
    }
}

Lambda alpha(const Lambda e, const string new_arg)
{
    if (lambda_type(e) == "assoc") {
        Lambda e2 = new_lambda();
        e2->arg = new_arg;
        e2->body = rename_references(e->body, e->arg, e2->arg);
        return e2;
    }
    else {
        alpha_error();
    }
}

Lambda substitute(const Lambda e, const string arg, const Lambda value) {
    string type = lambda_type(e);
    if (type == "var") {
        if (e->var == arg) {
            return value;
        }
        else {
            return e;
        }
    }
    else if (type == "assoc") {
        if (e->arg == arg) {
            return e;
        }
        else {
            Lambda new_body = substitute(e->body, arg, value);
            if (new_body != e->body) {
                if (find_reference(value, e->arg)) {
                    beta_error(); // name conflict
                }
                else {
                    Lambda e2 = new_lambda();
                    e2->arg = e->arg;
                    e2->body = new_body;
                    return e2;
                }
            }
            else {
                return e;
            }
        }
    }
    else if (type == "app") {
        Lambda new_left = substitute(e->left, arg, value);
        Lambda new_right = substitute(e->right, arg, value);
        if (new_left != e->left || new_right != e->right) {
            Lambda e2 = new_lambda();
            e2->left = new_left;
            e2->right = new_right;
            return e2;
        }
        else {
            return e;
        }
    }
    else {
        beta_error();
    }
}

Lambda beta(const Lambda e) {
    if (lambda_type(e) == "app") {
        if (lambda_type(e->left) == "assoc") {
            Lambda func = e->left;
            Lambda value = e->right;
            return substitute(func->body, func->arg, value);
        }
        else {
            return e;
        }
    }
    else {
        return e;
    }
}


// Utility functions.
ostream &operator<<(ostream &out, const Lambda e)
{
    string type = lambda_type(e);
    if (type == "var")
    {
        out << "var: " << e->var;
    }
    else if (type == "assoc")
    {
        out << "(\\"
            << "arg: " << e->arg << " "
            << "body: " << e->body << ")";
    }
    else if (type == "app")
    {
        out << "("
            << "left: " << e->left << " "
            << "right: " << e->right << ")";
    }
    else
    {
        out << "unknown";
    }
    return out;
}

bool operator==(const Lambda l, const Lambda r)
{
    if (l != nullptr && r != nullptr)
    {
        return (
            l->var == r->var &&
            l->arg == r->arg &&
            l->body == r->body &&
            l->left == r->left &&
            l->right == r->right);
    }
    else if (l == nullptr && r == nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}

string remove_spaces(const string &s)
{
    string result = "";
    for (char ch : s)
    {
        if (ch != ' ')
            result += ch;
    }
    return result;
}
