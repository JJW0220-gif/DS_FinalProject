#define FILE_EXTENSION ".txt"
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<iostream>
#include<algorithm>
#include <utility>  // 必須包含此標頭才能使用 std::pair
#include<unordered_set>
#include <chrono>

using namespace std;
const int NUMCHARS = 26;

struct TrieNode {
    TrieNode* child[NUMCHARS];
    unordered_set<int> fileIndices; // 紀錄來自的文件索引
    bool isEndOfWord;

    TrieNode() : isEndOfWord(false) {
        for (int i = 0; i < NUMCHARS; i++) {
            child[i] = nullptr;
        }
    }
};

class Trie {
public:
    Trie() { root = new TrieNode(); }

    ~Trie() { destroyTree(root); }

    // 插入字串並記錄來自的文件索引
    void insert(const string& word, int fileIndex) {
        TrieNode* node = root;
        int index;
        for (char ch : word) {
            if(ch >= 'a' && ch <= 'z')
                index = ch - 'a';
            else    index = ch - 'A';
            if (!node->child[index]) {
                node->child[index] = new TrieNode();
            }
            node = node->child[index];
        }
        node->isEndOfWord = true;
        node->fileIndices.insert(fileIndex);
    }

    // 查詢字串是否存在
    bool exactSearch(const string& word, unordered_set<int>& resultFiles) {
        TrieNode* node = root;
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->child[index]) {
                return false;
            }
            node = node->child[index];
        }
        if (node->isEndOfWord) {
            resultFiles = node->fileIndices;
            return true;
        }
        return false;
    }

    // 查詢前綴
    void prefixSearch(const string& prefix, vector<pair<string, unordered_set<int>>>& results) {
        TrieNode* node = root;
        for (char ch : prefix) {
            int index = ch - 'a';
            if (!node->child[index]) return;
            node = node->child[index];
        }
        dfs(node, prefix, results);
    }

    
    // 通配符查詢
    void wildcardSearch(const string& pattern, vector<pair<string, unordered_set<int>>>& results) {
        wildcardHelper(root, pattern, 0, "", results);
    }

    void print() const {
        printTrieHelper(root, "");
    }

private:
    TrieNode* root;

    void dfs(TrieNode* node, const string& prefix, vector<pair<string, unordered_set<int>>>& results) {
        if (!node) return;
        if (node->isEndOfWord) {
            results.emplace_back(prefix, node->fileIndices);
        }
        for (int i = 0; i < NUMCHARS; i++) {
            if (node->child[i]) {
                dfs(node->child[i], prefix + char(i + 'a'), results);
            }
        }
    }


    void wildcardHelper(TrieNode* node, const string& pattern, int pos, const string& current, vector<pair<string, unordered_set<int>>>& results) {
        if (!node) return;

        // 如果達到字串尾部，檢查是否是單詞結尾
        if (pos == pattern.size()) {
            if (node->isEndOfWord) {
                results.emplace_back(current, node->fileIndices);
            }
            return;
        }

        if (pattern[pos] == '*') {
            // 處理連續的 `*`
            while (pos < pattern.size() && pattern[pos] == '*') {
                pos++;
            }
            
            // 嘗試匹配 0 個字元
            wildcardHelper(node, pattern, pos, current, results);

            // 嘗試匹配 1 或多個字元
            for (int i = 0; i < NUMCHARS; i++) {
                if (node->child[i]) {
                    wildcardHelper(node->child[i], pattern, pos - 1, current + char(i + 'a'), results);
                }
            }
        } else {
            // 精確匹配當前字元
            int index = pattern[pos] - 'a';
            if (index >= 0 && index < NUMCHARS && node->child[index]) {
                wildcardHelper(node->child[index], pattern, pos + 1, current + pattern[pos], results);
            }
        }
    }


    /*void wildcardHelper(TrieNode* node, const string& pattern, int pos, const string& current, vector<pair<string, unordered_set<int>>>& results) {
        if (!node) return;

        if (pos == pattern.size()) {
            if (node->isEndOfWord) {
                results.emplace_back(current, node->fileIndices);
            }
            return;
        }

        if (pattern[pos] == '*') {
            // 匹配 0 或多個字元
            wildcardHelper(node, pattern, pos + 1, current, results); // 匹配 0 個字元
            for (int i = 0; i < NUMCHARS; i++) {
                if (node->child[i]) {
                    wildcardHelper(node->child[i], pattern, pos, current + char(i + 'a'), results);
                }
            }
        } else {
            // 精確匹配當前字元
            int index = pattern[pos] - 'a';
            if (index >= 0 && index < NUMCHARS && node->child[index]) {
                wildcardHelper(node->child[index], pattern, pos + 1, current + pattern[pos], results);
            }
        }
    }*/

    void printTrieHelper(TrieNode* node, const string& current) const {
        if (!node) return;

        if (node->isEndOfWord) {
            cout << "Word: " << current << ", Files: ";
            for (int fileIndex : node->fileIndices) {
                cout << fileIndex << " ";
            }
            cout << endl;
        }

        for (int i = 0; i < NUMCHARS; i++) {
            if (node->child[i]) {
                printTrieHelper(node->child[i], current + char('a' + i));
            }
        }
    }

    void destroyTree(TrieNode* node) {
        if (!node) return;
        for (int i = 0; i < NUMCHARS; i++) {
            destroyTree(node->child[i]);
        }
        delete node;
    }
    
};


// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string){
	vector<string> parse_string;
	for(auto& word : tmp_string){
		string new_str;
    	for(auto &ch : word){
			if(isalpha(ch))
				new_str.push_back(ch);
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;

	char * strs = new char[str.length() + 1] ; 
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		string s = p; 
		res.push_back(s); 
		p = strtok(NULL, d);
	}

	return res;
}




int main(int argc, char *argv[])
{

    auto start = std::chrono::high_resolution_clock::now();

    vector<string> dict;        // save title
    // INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route
    Trie trie, trie_rev;
    string data_dir = argv[1] + string("\\");
	string query = string(argv[2]);
	string output = string(argv[3]);

    ofstream out(output);
    if (!out.is_open()) {
        cerr << "Error: Unable to open output file." << endl;
        return 1;
    }


    // Read query file and insert queries into the Trie
    
    
	// Read File & Parser Example
	string file, title_name, tmp;
	fstream fi;
	vector<string> tmp_string;

    // from data_dir get file ....
	// eg : use 0.txt in data directory
    int i = 0;
	while(true){
		string check = to_string(i);
		fi.open(data_dir+check+".txt", ios::in);
        //cerr << data_dir+check+".txt" << endl;
        if(!fi) {
            cerr << "no file" << i<< endl;
            break;
        }
		if (!fi.is_open()){
			cerr<<"not open data" + check <<endl;
            break;
        }


		// GET TITLENAME
		getline(fi, title_name);
		// GET TITLENAME WORD ARRAY
		tmp_string = split(title_name, " ");
		vector<string> title = word_parse(tmp_string);
        dict.push_back(title_name);         //jj
        vector<string> see;
		for(auto &word : title){
            trie.insert(word, i);
            reverse(word.begin(), word.end());
            trie_rev.insert(word, i);
        }


		// GET CONTENT LINE BY LINE

		while(getline(fi, tmp)){

			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");

			// PARSE CONTENT
			vector<string> content = word_parse(tmp_string);

			for(auto &word : content){
				trie.insert(word, i);
                reverse(word.begin(), word.end());
                trie_rev.insert(word, i);
			}
			
        }


		// CLOSE FILE
		fi.close();

        i++;

	}
    cout<<i<<endl;

    ifstream query_in(query);
    if (!query_in.is_open()) {
        cerr << "Error: Unable to open query file." << endl;
    }

    //trie.print();
    //trie_rev.print();

    bool* rec = new bool[i+1];
    bool* temp = new bool[i+1];
    for (int j = 0; j < i + 1; j++) {
        rec[j] = false; 
        temp[j] = false;
    }

    int see = 0;
    string query_line, words = "";
    int state = 0, state2 = 0, now = 0;
    while (getline(query_in, query_line)) {
        //cout<<endl;
        
        int index = 0;
        for(char ch: query_line){
            index += 1;
            //cout << ch << state2;
            if(state2 == 0){
                if(ch == '"'){
                    now = 1;
                } else if ( ch == '<'){
                    now = 2;
                } else if( ch == '*'){
                    now = 3;
                } else if(ch == ' '){
                    
                } else {
                    now == 4;
                    words += ch;
                }
                state2 = 1;
            } else if(state2 == 1){
                //cout<<words<<" " << now << " " << ch << endl;
                if(ch == '*' && now == 3){
                    if(state == 1)  see = 1;
                    //cout<<words << state <<endl;
                    reverse(words.begin(), words.end());
                    vector<pair<string, unordered_set<int>>> prefixResults;
                    trie_rev.prefixSearch(words, prefixResults);
                    for (const auto& pair : prefixResults) {
                        const auto& word = pair.first;
                        const auto& files = pair.second;
                        for (int fileIndex : files) {
                            if(state == 0){
                                rec[fileIndex] = true;
                                //cout << fileIndex << " ";
                            } else if(state == 1){
                                //cout << fileIndex << " ";
                                temp[fileIndex] = true;
                            } else if(state == 2){
                                rec[fileIndex] = false;
                            }
                        }
                    }
                    
                    state2 = 2;
                } else if(ch == '"'){
                    if(state == 1)  see = 1;
                    //cout<<words;
                    //cout << "thisis"<<endl;
                    unordered_set<int> resultFiles;
                    if (trie.exactSearch(words, resultFiles)) {
                        for (int fileIndex : resultFiles){
                            if(state == 0){
                                //cout << fileIndex << "dsv"<< endl;
                                rec[fileIndex] = true;
                            }
                            else if(state == 1){
                                see = 1;
                                //cout << fileIndex;
                                temp[fileIndex] = true;
                            } else if(state == 2){
                                rec[fileIndex] = false;
                            }
                        }
                    } /*else {
                        cout << "nofile_fit" <<endl;
                    }*/
                    state2 = 2;
                    
                } else if(ch == '>'){
                    if(state == 1)  see = 1;
                    //cout<<words;
                    vector<pair<string, unordered_set<int>>> wildcardResults;
                    trie.wildcardSearch(words, wildcardResults);
                    for (const auto& pair : wildcardResults) {
                        const auto& word = pair.first;
                        const auto& files = pair.second;
                        for (int fileIndex : files) {
                            if(state == 0)
                                rec[fileIndex] = true;
                            else if(state == 1){
                                see = 1;
                                //cout << fileIndex;
                                temp[fileIndex] = true;
                            } else if(state == 2){
                                rec[fileIndex] = false;
                            }
                        }
                    }
                    state2 = 2;
                } else if( ch == ' '){
                    if(state == 1)  see = 1;
                    //cout<<words;
                    vector<pair<string, unordered_set<int>>> prefixResults;
                    trie.prefixSearch(words, prefixResults);
                    for (const auto& pair : prefixResults) {
                        const auto& word = pair.first;
                        const auto& files = pair.second;
                        for (int fileIndex : files) {
                            if(state == 0)
                                rec[fileIndex] = true;
                            else if(state == 1){
                                see = 1;
                                //cout << fileIndex;
                                temp[fileIndex] = true;
                            } else if(state == 2){
                                rec[fileIndex] = false;
                            }
                        }
                    }
                    state2 = 2;
                }
                else {
                    words += ch;
                    //cout<<words;
                    if(query_line.size()  == index){
                        //cout<<words;
                        vector<pair<string, unordered_set<int>>> prefixResults;
                        trie.prefixSearch(words, prefixResults);
                        for (const auto& pair : prefixResults) {
                            const auto& word = pair.first;
                            const auto& files = pair.second;
                            for (int fileIndex : files) {
                                
                                if(state == 0)
                                    rec[fileIndex] = true;
                                else if(state == 1){
                                    //cout << fileIndex;
                                    see = 1;
                                    temp[fileIndex] = true;
                                } else if(state == 2){
                                    rec[fileIndex] = false;
                                }
                            }
                        }
                    }
                }
                //cout << state << see <<endl;

                if(state == 1 && see == 1){
                    for (int j = 0; j < i + 1; j++) {
                        //if(rec[j] == true)      cout << j << "jb" <<endl;
                        rec[j] = rec[j] && temp[j];
                        //if(rec[j] == true)      cout << j << "js" <<endl;
                        temp[j] = false;
                    }
                    see = 0;
                    
                }
                /*for (int j = 0; j < i + 1; j++) {
                    
                    if(rec[j] == true)      cout << j << words  <<endl;
                   
                }*/
                
            } else if(state2 == 2){
                if(ch == ' '){
                    
                } if(ch == '/'){
                    state = 0;
                } else if(ch =='+'){
                    state = 1;
                } else if(ch == '-'){
                    state = 2;
                } else {
                    //cout << "error" <<ch;
                }
                words = "";
                state2 = 3;
            } else if(state2 == 3){
                //cout << ch<<' '<<endl;
                if(ch == '/'){
                    state = 0;
                } else if(ch =='+'){
                    state = 1;
                } else if(ch == '-'){
                    state = 2;
                } else if(ch == ' '){
                    state2 = 0;
                    //cout << 0;
                } else {
                    cout<< "error ch "<< ch <<endl;
                }
            }
            
        }
        state2 = 0;
        state = 0;
        now = 0;
        words = "";
        
        int check = 0;
        //out << query_line << endl <<endl;
        for (int j = 0; j < i + 1; j++) {
            
            if(rec[j] == true){
                //cout << j;
                check = 1;
                //cout << j <<"see";
                out<<dict[j]/*<< " " << query_line<<" "<< j */<<endl;
                rec[j] = false;
            }
            temp[j] = false;
            //cout<<endl;
        }

        if(check == 0){
            out << "Not Found!" << endl;
        }
    }


    
    query_in.close();


    auto end = std::chrono::high_resolution_clock::now();

    // 計算耗時（以毫秒為單位）
    chrono::duration<double, std::milli> elapsed = end - start;

    cout << "run time: " << elapsed.count() << " ms" << endl;
	
}


// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C

//
