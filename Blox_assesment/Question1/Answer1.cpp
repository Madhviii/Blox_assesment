#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>
#include <gmp.h>     // Include C GMP header
#include <gmpxx.h>   // Include C++ GMP header for mpz_class and mpf_class

using namespace std;


enum class ValueType {
    INTEGER,
    FLOAT,
    STRING,
    OBJECT,
    ARRAY,
    BOOLEAN,
    NIL
};


class Value {
public:
    ValueType type;
    mpz_class integerValue;     // Arbitrary precision integer
    mpf_class floatValue;       // Arbitrary precision float
    string stringValue;
    map<string, Value> objectValue;
    vector<Value> arrayValue;
    bool boolValue;

    // Constructors for each type
    Value() : type(ValueType::NIL) {}
    Value(mpz_class val) : type(ValueType::INTEGER), integerValue(val) {}
    Value(mpf_class val) : type(ValueType::FLOAT), floatValue(val) {}
    Value(const string& val) : type(ValueType::STRING), stringValue(val) {}
    Value(bool val) : type(ValueType::BOOLEAN), boolValue(val) {}
    Value(const map<string, Value>& val) : type(ValueType::OBJECT), objectValue(val) {}
    Value(const vector<Value>& val) : type(ValueType::ARRAY), arrayValue(val) {}

    // Function to print the value
    void print() const {
        switch (type) {
            case ValueType::INTEGER:
                cout << integerValue.get_str(); // Use get_str() for mpz_class
                break;
            case ValueType::FLOAT: {
                mp_exp_t exp;
                cout << floatValue.get_str(exp); // Use get_str() with an exponent
                break;
            }
            case ValueType::STRING:
                cout << "\"" << stringValue << "\"";
                break;
            case ValueType::OBJECT: {
                cout << "{";
                for (auto it = objectValue.begin(); it != objectValue.end(); ++it) {
                    if (it != objectValue.begin()) cout << ", ";
                    cout << "\"" << it->first << "\": ";
                    it->second.print();
                }
                cout << "}";
                break;
            }
            case ValueType::ARRAY: {
                cout << "[";
                for (size_t i = 0; i < arrayValue.size(); ++i) {
                    if (i > 0) cout << ", ";
                    arrayValue[i].print();
                }
                cout << "]";
                break;
            }
            case ValueType::BOOLEAN:
                cout << (boolValue ? "true" : "false");
                break;
            case ValueType::NIL:
                cout << "null";
                break;
        }
    }
};

// Forward declarations
Value parseValue(const string& json, size_t& pos);
map<string, Value> parseObject(const string& json, size_t& pos);
vector<Value> parseArray(const string& json, size_t& pos);

// Utility functions to skip whitespace and check characters
void skipWhitespace(const string& json, size_t& pos) {
    while (pos < json.size() && isspace(json[pos])) ++pos;
}

bool isDigit(char c) {
    return (c >= '0' && c <= '9');
}


map<string, Value> parseObject(const string& json, size_t& pos) {
    map<string, Value> obj;
    ++pos; // Skip '{'
    skipWhitespace(json, pos);
    while (pos < json.size() && json[pos] != '}') {
        skipWhitespace(json, pos);
        if (json[pos] == ',') ++pos; // Skip comma
        skipWhitespace(json, pos);

        // Parse key
        if (json[pos] != '"') throw runtime_error("Expected string key");
        size_t start = ++pos;
        while (pos < json.size() && json[pos] != '"') ++pos;
        string key = json.substr(start, pos - start);
        ++pos; // Skip closing '"'
        
        skipWhitespace(json, pos);
        if (json[pos] != ':') throw runtime_error("Expected ':' after key");
        ++pos; // Skip ':'
        skipWhitespace(json, pos);

        // Parse value
        Value value = parseValue(json, pos);
        obj[key] = value;

        skipWhitespace(json, pos);
    }
    if (json[pos] != '}') throw runtime_error("Expected '}' at the end of object");
    ++pos; // Skip '}'
    return obj;
}

// Parse a JSON array
vector<Value> parseArray(const string& json, size_t& pos) {
    vector<Value> arr;
    ++pos; // Skip '['
    skipWhitespace(json, pos);
    while (pos < json.size() && json[pos] != ']') {
        if (json[pos] == ',') ++pos; // Skip comma
        skipWhitespace(json, pos);

        // Parse value
        Value value = parseValue(json, pos);
        arr.push_back(value);

        skipWhitespace(json, pos);
    }
    if (json[pos] != ']') throw runtime_error("Expected ']' at the end of array");
    ++pos; // Skip ']'
    return arr;
}

// Parse a JSON value
Value parseValue(const string& json, size_t& pos) {
    skipWhitespace(json, pos);
    if (json[pos] == '"') {
        // String value
        size_t start = ++pos;
        while (pos < json.size() && json[pos] != '"') ++pos;
        string str = json.substr(start, pos - start);
        ++pos; // Skip closing '"'
        return Value(str);
    } else if (json[pos] == '{') {
        // Object value
        return Value(parseObject(json, pos));
    } else if (json[pos] == '[') {
        // Array value
        return Value(parseArray(json, pos));
    } else if (json[pos] == 't' && json.substr(pos, 4) == "true") {
        pos += 4;
        return Value(true);
    } else if (json[pos] == 'f' && json.substr(pos, 5) == "false") {
        pos += 5;
        return Value(false);
    } else if (json[pos] == 'n' && json.substr(pos, 4) == "null") {
        pos += 4;
        return Value();
    } else if (isDigit(json[pos]) || json[pos] == '-') {
        // Number value (integer or float)
        size_t start = pos;
        bool isFloat = false;
        if (json[pos] == '-') ++pos;
        while (pos < json.size() && (isDigit(json[pos]) || json[pos] == '.')) {
            if (json[pos] == '.') isFloat = true;
            ++pos;
        }
        string numStr = json.substr(start, pos - start);
        if (isFloat) {
            return Value(mpf_class(numStr));
        } else {
            return Value(mpz_class(numStr));
        }
    }
    throw runtime_error("Invalid JSON value");
}


Value parseJSON(const string& json) {
    size_t pos = 0;
    return parseValue(json, pos);
}

// Sample usage
int main() {
    string jsonString = R"({"name": "John", "age": 12, "height": 1.23, "is_student": false, "courses": ["Math", "Science"], "address": {"city": "Pune", "zip": 10001}})";
    try {
        Value parsedJSON = parseJSON(jsonString);
        cout << "JSON parsed successfully!" << endl;
        parsedJSON.print(); // Print the parsed JSON
        cout << endl; // For better output formatting
    } catch (const exception& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
    }
    return 0;
}
