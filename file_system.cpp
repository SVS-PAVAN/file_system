#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct TrieNode {
  map<string, shared_ptr<TrieNode>> children; 
  bool isFile = false;
  string content;
};

class FileSystem {
public:
  FileSystem() {
    currentPath = "/";
  }

  vector<string> ls() {
    auto [node, lastDir] = createDirAndGetPair(currentPath);
    if (node->isFile)
      return {lastDir};

    vector<string> ans;

    for (const auto& [file, _] : node->children)
      ans.push_back(file);

    return ans;
  }

  void mkdir(string path) {
    createDirAndGetPair(path);
  }

  void touch() {
    auto [node, filename] = createDirAndGetPair(currentPath);
    node->children[filename] = make_shared<TrieNode>();
    node->children[filename]->isFile = true;
    node->children[filename]->content = "";
  }

  void addContent(string filePath, string content) {
    shared_ptr<TrieNode> node = createDirAndGetPair(filePath).first;
    node->isFile = true;
    node->content += content;
  }

  string cat(string filePath) {
    shared_ptr<TrieNode> node = createDirAndGetPair(filePath).first;
    return node->content;
  }

  void mv(string dest) {
    auto [srcNode, _] = createDirAndGetPair(currentPath);
    auto [destNode, destDir] = createDirAndGetPair(dest);
    destNode->children[destDir] = srcNode;
    srcNode->children.erase(destDir);
  }

  void cp(string dest) {
    auto [srcNode, _] = createDirAndGetPair(currentPath);
    auto [destNode, destDir] = createDirAndGetPair(dest);
    destNode->children[destDir] = srcNode;
  }

  void rm() {
    auto [node, _] = createDirAndGetPair(currentPath);
    node->isFile = false;
    node->content = "";
  }

  void cd(string path) {
    if (path == "/") {
      currentPath = "/";
      return;
    }

    vector<string> dirs = getDirs(path);
    shared_ptr<TrieNode> node = root;

    for (const string& dir : dirs) {
      if (!node->children.count(dir))
        return;  // Directory doesn't exist
      node = node->children[dir];
    }

    currentPath = path;
  }

  void echo(string content) {
    shared_ptr<TrieNode> node = createDirAndGetPair(currentPath).first;
    node->isFile = true;
    node->content = content;
  }

public:
  shared_ptr<TrieNode> root = make_shared<TrieNode>();
  string currentPath;

  pair<shared_ptr<TrieNode>, string> createDirAndGetPair(const string& path) {
    const vector<string> dirs = getDirs(path);
    shared_ptr<TrieNode> node = root;

    for (const string& dir : dirs) {
      if (!node->children.count(dir))
        node->children[dir] = make_shared<TrieNode>();
      node = node->children[dir];
    }

    return {node, dirs.empty() ? "" : dirs.back()};
  }

  vector<string> getDirs(const string& path) {
    vector<string> dirs;
    istringstream iss(path);

    for (string dir; getline(iss, dir, '/');)
      if (!dir.empty()) 
        dirs.push_back(dir);

    return dirs;
  }
};

int main() {
  FileSystem fileSystem;
  string command, content, filePath;

  cout << "Welcome to Custom File System!" << endl;

  while (true) {
    cout << "\nEnter command: ";
    cin >> command;

    if (command == "mkdir") {
      cin >> filePath;
      fileSystem.mkdir(filePath);
    } else if (command == "touch") {
      fileSystem.touch();
    } else if (command == "ls") {
      vector<string> dirContent = fileSystem.ls();
      cout << "Contents of " << fileSystem.currentPath << ":" << endl;
      for (const auto& file : dirContent)
        cout << file << endl;
    } else if (command == "addContent") {
      cin >> filePath>> content;
      fileSystem.addContent(filePath, content);
    } else if (command == "cat") {
      cin >> filePath;
      cout << "Contents: " << fileSystem.cat(filePath) << endl;
    } else if (command == "mv") {
      cin >> filePath;
      fileSystem.mv(filePath);
    } else if (command == "cp") {
      cin >> filePath;
      fileSystem.cp(filePath);
    } else if (command == "rm") {
      fileSystem.rm();
    } else if (command == "cd") {
      cin >> filePath;
      fileSystem.cd(filePath);
    } else if (command == "echo") {
      getline(cin, content);
      fileSystem.echo(content);
    } else if (command == "exit") {
      break;
    } else {
      cout << "Invalid command! Try again." << endl;
    }
  }

  cout << "Exiting Custom File System..." << endl;

  return 0;
}
