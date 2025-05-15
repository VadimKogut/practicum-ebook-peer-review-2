#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;

class Domain {
public:
    Domain() = default;

    Domain(string str) {
        size_t start = 0;
        size_t end = str.find('.');
        
        while (end != string::npos) {
            elements_.push_back(str.substr(start, end - start));
            start = end + 1;
            end = str.find('.', start);
        }
        elements_.push_back(str.substr(start));
        
        // Храним домен в обратном порядке для эффективной проверки
        reverse(elements_.begin(), elements_.end());
    }

    bool operator==(const Domain& other) const {
        return elements_ == other.elements_;
    }

    const vector<string>& GetElements() const {
        return elements_;
    }

private:
    vector<string> elements_;
};

struct TrieNode {
    unordered_map<string, unique_ptr<TrieNode>> children;
    bool is_end = false;
};

class DomainTrie {
public:
    DomainTrie() : root(make_unique<TrieNode>()) {}

    void AddDomain(const Domain& domain) {
        TrieNode* current = root.get();
        for (const auto& part : domain.GetElements()) {
            if (current->children.find(part) == current->children.end()) {
                current->children[part] = make_unique<TrieNode>();
            }
            current = current->children[part].get();
        }
        current->is_end = true;
    }

    bool ContainsSubdomain(const Domain& domain) const {
        const TrieNode* current = root.get();
        for (const auto& part : domain.GetElements()) {
            if (current->children.find(part) == current->children.end()) {
                return false;
            }
            current = current->children.at(part).get();
            if (current->is_end) {
                return true;
            }
        }
        return current->is_end;
    }

private:
    unique_ptr<TrieNode> root;
};

class DomainChecker {
public:
    template <typename Iterator>
    DomainChecker(Iterator begin, Iterator end) {
        for (auto it = begin; it != end; ++it) {
            trie_.AddDomain(*it);
        }
    }

    bool IsForbidden(const Domain& domain) const {
        return trie_.ContainsSubdomain(domain);
    }

private:
    DomainTrie trie_;
};

template <typename Number>
Number ReadNumberOnLine(istream& input) {
    string line;
    getline(input, line);

    Number num;
    istringstream(line) >> num;

    return num;
}

vector<Domain> ReadDomains(istream& input, size_t count_domains) {
    vector<Domain> domains;
    domains.reserve(count_domains);

    string domain;
    for (size_t i = 0; i < count_domains; ) {
        if (!getline(input, domain)) {
            break;
        }

        if (domain.empty()) {
            continue;
        }

        domains.emplace_back(move(domain));
        ++i;
    }

    return domains;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    const vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << endl;
    }
}
