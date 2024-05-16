//Ahmed Baha Eddine Alimi
#include <bits/stdc++.h>
 
 
using namespace std;
 
class FiniteAutomatonTranslator {
private:
    bool hasError;
    string exception1, emptyAcceptingException;
    bool malFormedException, initialStateException, disjointException, deterministicException;
    vector<string> states, alphabet, initState, finState, transitionss;
    ifstream inputFile;
    int n;
    string type;
 
    void breadthFirstSearch(int start, vector<vector<int>>& graph, vector<bool>& visited, vector<int>& previous) {
        queue<int> q;
        q.push(start);
        visited[start] = true;
 
        while (!q.empty()) {
            int current = q.front();
            q.pop();
 
            for (int vertex : graph[current]) {
                if (!visited[vertex]) {
                    visited[vertex] = true;
                    previous[vertex] = current;
                    q.push(vertex);
                }
            }
        }
    }
 
public:
    FiniteAutomatonTranslator(const string& fileNameIn) {
        hasError = false;
        malFormedException = initialStateException   = disjointException = deterministicException = false;
        inputFile.open(fileNameIn);
 
        // E0: Input file is malformed
        for (int i = 0; i < 6; ++i) {
            string command;
            getline(inputFile, command);
            size_t start = command.find('[');
            size_t end = command.find(']');
            string commandNew = command.substr(start + 1, end - start - 1);
            if (i == 0) {
                stringstream ss(commandNew);
                string type;
                getline(ss, type, '=');
                getline(ss, type, '=');
                this->type = type;
                if ((this->type != "deterministic") && (this->type != "non-deterministic")) {
                    malFormedException = true;
                    hasError = true;
                }
            }  else if (i == 1) {
                stringstream ss(commandNew);
                string state;
                while (getline(ss, state, ',')) {
                    if (find(states.begin(), states.end(), state) == states.end()) {
                        states.push_back(state);
                    }
                }
                n = states.size();
                if (states.empty()) {
                    malFormedException = true;
                    hasError = true;
                    cout << "E1: Input file is malformed";
                    exit(0);
                }
                if (command.find("states=[") == string::npos || end == string::npos) {
                    cout << "E1: Input file is malformed";
                    exit(0);
                }
            } else if (i == 2) {
                stringstream ss(commandNew);
                string symbol;
                while (getline(ss, symbol, ',')) {
                    alphabet.push_back(symbol);
                }
                if (!commandNew.empty() && commandNew.back() == ',') {
                    malFormedException = true;
                    hasError = true;
                    cout << "E1: Input file is malformed";
                    exit(0);
                }
                if (alphabet.empty()) {
                    malFormedException = true;
                    hasError = true;
                    cout << "E1: Input file is malformed";
                    exit(0);
                }
                if (command.find("alphabet=[") == string::npos || end == string::npos) {
                    malFormedException = true;
                    hasError = true;
                }
            } else if (i == 4) {
                stringstream ss(commandNew);
                string final;
                while (getline(ss, final, ',')) {
                    finState.push_back(final);
                }
                sort(finState.begin(), finState.end());
                if (command.find("accepting=[") == string::npos || end == string::npos) {
                    malFormedException = true;
                    hasError = true;
                } else if (finState.empty()) {
                    emptyAcceptingException = "E3: Set of accepting states is empty";
                    hasError = true;
                }
            } else if (i == 5) {
                stringstream ss(commandNew);
                string transition;
                set<string> uniqueTransitions;
                while (getline(ss, transition, ',')) {
                    if (transition.empty() || transition.find(">>") != string::npos) {
                        malFormedException = true;
                        hasError = true;
                        cout << "E1: Input file is malformed";
                        exit(0);
                    }
                    if (uniqueTransitions.count(transition) > 0) {
                        malFormedException = true;
                        hasError = true;
                        cout << "E1: Input file is malformed";
                        exit(0);
                    } else {
                        uniqueTransitions.insert(transition);
                    }
                    transitionss.push_back(transition);
                }
                if (command.find("transitions=[") == string::npos || end == string::npos) {
                    malFormedException = true;
                    hasError = true;
                }
            }else if (i == 3) {
                stringstream ss(commandNew);
                string initial;
                while (getline(ss, initial, ',')) {
                    initState.push_back(initial);
                }
                if (command.find("initial=[") == string::npos || end == string::npos) {
                    malFormedException = true;
                    hasError = true;
                }
                for (const string& init : initState) {
                    if (find(states.begin(), states.end(), init) == states.end()) {
                        exception1 = init;
                        cout<<"E4: A state '" + exception1 + "' is not in the set of states";
                        exit(0);
                    }
                }
            }
        }
    }
 
    bool foundError() {
        return hasError;
    }
 
    void outputErrors() {
        string errorMessage = "";
        if (malFormedException) {
            errorMessage += "E1: Input file is malformed";
        } else if (initialStateException) {
            errorMessage += "E2: Initial state is not defined";
        } else if (!emptyAcceptingException.empty()) { // Check if emptyAcceptingException is not empty
            errorMessage += emptyAcceptingException;
        } else if (!exception1.empty()) {
            errorMessage += "E4: A state '" + exception1 + "' is not in the set of states";
        } else if (!emptyAcceptingException.empty()) {
            errorMessage += "E5: A transition '" + emptyAcceptingException + "' is not represented in the alphabet";
        } else if (disjointException) {
            errorMessage += "E6: Some states are disjoint";
        } else if (deterministicException) {
            errorMessage += "E7: FSA is non-deterministic";
        }
        cout << errorMessage;
    }
 
    void check() {
        vector<bool> visited(n, false);
        vector<int> prev(n, -1);
        vector<vector<int>> graph(n);
 
        // E2: Initial state is not defined
        if (initState.empty()) {
            initialStateException = true;
            hasError = true;
        }
 
        // E4: A state 's' is not in the set of states
        for (const string& init : initState) {
            if (find(states.begin(), states.end(), init) == states.end()) {
                exception1 = init;
                hasError = true;
                break;
            }
        }
 
        // Constructing graph and checking for E5
        if (!hasError) {
            for (const string& tran : transitionss) {
                stringstream ss(tran);
                string s1, a, s2;
                getline(ss, s1, '>');
                getline(ss, a, '>');
                getline(ss, s2, '>');
                graph[distance(states.begin(), find(states.begin(), states.end(), s1))].push_back(distance(states.begin(), find(states.begin(), states.end(), s2)));
                if (find(states.begin(), states.end(), s1) == states.end()) {
                    exception1 = s1;
                    hasError = true;
                    break;
                }
                if (find(states.begin(), states.end(), s2) == states.end()) {
                    exception1 = s2;
                    hasError = true;
                    break;
                }
                if (find(alphabet.begin(), alphabet.end(), a) == alphabet.end()) {
                    emptyAcceptingException = "E5: A transition '" + a + "' is not represented in the alphabet";
                    hasError = true;
                    break;
                }
            }
        }
 
        // E6: Some states are disjoint
        breadthFirstSearch(0, graph, visited, prev);
        if (find(visited.begin(), visited.end(), false) != visited.end()) {
            disjointException = true;
            hasError = true;
        }
 
        // E7: FSA is non-deterministic
        if (type == "deterministic") {
            set<pair<string, string>> transitions;
            for (const string& tran : transitionss) {
                stringstream ss(tran);
                string s1, a, s2;
                getline(ss, s1, '>');
                getline(ss, a, '>');
                getline(ss, s2, '>');
                if (transitions.count({s1, a}) > 0) {
                    deterministicException = true;
                    hasError = true;
                    break;
                } else {
                    transitions.insert({s1, a});
                }
            }
        }
    }
 
    void toRegExp() {
        vector<int> finalStatesIndex;
        vector<vector<string>> regex = privateGetInitialRegex(finalStatesIndex);
 
        for (int k = 0; k < states.size(); ++k) {
            vector<vector<string>> newRegex(states.size(), vector<string>(states.size(), ""));
            for (int i = 0; i < states.size(); ++i) {
                for (int j = 0; j < states.size(); ++j) {
                    newRegex[i][j] = "(" + regex[i][k] + ")(" + regex[k][k] + ")*(" + regex[k][j] + ")|(" + regex[i][j] + ")";
                }
            }
            regex = newRegex;
        }
 
        // Construct the final regular expression with parentheses
        string resultNewRegex;
        for (int i = 0; i < finalStatesIndex.size(); ++i) {
            if (!regex[0][finalStatesIndex[i]].empty()) {
                if (!resultNewRegex.empty()) {
                    resultNewRegex += "|";
                }
                resultNewRegex += "(" + regex[0][finalStatesIndex[i]] + ")";
            }
        }
 
        // If no final states, set result to empty set
        if (resultNewRegex.empty()) {
            resultNewRegex = "{}";
        }
 
        cout << resultNewRegex;
    }
 
private:
    vector<vector<string>> privateGetInitialRegex(vector<int>& finStatesInd) {
        vector<vector<string>> initRegex(states.size(), vector<string>(states.size(), ""));
 
        for (int i = 0; i < states.size(); ++i) {
            string state = states[i];
            if (find(finState.begin(), finState.end(), state) != finState.end()) {
                finStatesInd.push_back(i);
            }
        }
 
        for (int i = 0; i < states.size(); ++i) {
            string state = states[i];
 
            for (int j = 0; j < states.size(); ++j) {
                string newState = states[j];
                string regex = "";
                for (const string& transition : transitionss) {
                    stringstream ss(transition);
                    string s1, a, s2;
                    getline(ss, s1, '>');
                    getline(ss, a, '>');
                    getline(ss, s2, '>');
                    if (s1 == state && s2 == newState) {
                        regex += a + "|";
                    }
                }
                if (state == newState) {
                    regex += "eps";
                }
                if (regex.empty()) {
                    regex = "{}";
                }
                if (regex.back() == '|') {
                    regex.pop_back();
                }
                initRegex[i][j] = regex;
            }
        }
        return initRegex;
    }
};
 
int main() {
    FiniteAutomatonTranslator FSATranslator("input.txt");
    FSATranslator.check();
 
    if (FSATranslator.foundError()) {
        FSATranslator.outputErrors();
    } else {
        FSATranslator.toRegExp();
    }
    return 0;
}