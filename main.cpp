#include<iostream>
#include<cstdlib>
#include<vector>
#include<ctime>
#include<fstream>

std::string delimiter = ":";

template<typename K, typename V>
class Node {
public:
    Node();
    ~Node();
    Node(K k, V v);
    K get_key();
    V get_value();
private:
    K _key;
    V _value;
};

template<typename K, typename V>
Node<K, V>::Node(K key, V value) {
    this->_key = key;
    this->_value = value;
}

template<typename K, typename V>
Node<K, V>::~Node() {}

template<typename K, typename V>
K Node<K,V>::get_key() {
    return this->_key;
}

template<typename K, typename V>
V Node<K,V>::get_value(){
    return this->_value;
}

template<typename K, typename V>
struct ListNode {
    Node<K,V> *val;
    ListNode *pre;
    ListNode *next;
    ListNode *down;
    ListNode(Node<K,V> *x) : val(x), pre(NULL), next(NULL), down(NULL) {}
    // ListNode(Node<K,V> *x, ListNode *next, ListNode *down) : val(x), pre(NULL) ,next(next), tail(next), down(down) {}
    ListNode(K k, V v, ListNode *pre = NULL, ListNode *next = NULL, ListNode *down=NULL) {
        this->val = new Node<K,V>(k,v);
        this->pre = pre;
        this->next = next;
        this->down = down;
    }
};

template<typename K, typename V>
class SkipList {
public:
    SkipList(); // 初始化跳表头结点
    ~SkipList(); 
    std::vector<ListNode<K, V>* > _head; // 头结点
    void print(); // 打印跳表结构(包括头结点)
    int put(K, V); // 插入key到跳表中, 插入成功返回1, 插入失败返回-1
    int del(K); // 从跳表中删除key, 删除成功返回1, 删除失败返回-1
    std::string get(K); // 返回key是否存在跳表中, 如果存在返回1, 不存在返回-1
    int length(); // 返回跳表结点个数
    void save_data(); // 持久化跳表
    void load_data(); // 加载持久化

private:
    // 返回最底层 小于key的前一个位置
    ListNode<K, V>* find_far_right_node_in_list(K key);
    // 返回当前层 小于key的前一个位置
    ListNode<K, V>* find_far_right_node_in_current_level(ListNode<K, V> *cur, K key);
    void delete_current_level_nodes(ListNode<K,V>* head);
    void delete_empty_level();
    void split_key_value(std::string str, std::string* key, std::string* value);
    int _size; // 链表长度(不包含头结点)
    int _max_level; // 跳表高度
};

template<typename K, typename V>
int SkipList<K, V>::length() {
    return this->_size;
}

template<typename K, typename V>
ListNode<K, V>* SkipList<K, V>::find_far_right_node_in_current_level(ListNode<K, V> *head, K key) {
    ListNode<K, V> *cur = head;
    while(cur->next != NULL && cur->next->val->get_key() < key) {
        cur = cur->next;
    }
    return cur;
}

template<typename K, typename V>
ListNode<K, V>* SkipList<K, V>::find_far_right_node_in_list(K key) {
    ListNode<K, V> *cur = this->_head[this->_max_level - 1];
    while(cur != NULL && cur->down != NULL) {
        cur = this->find_far_right_node_in_current_level(cur, key);
        if(cur->down != NULL) {
            cur = cur->down;
        }
    }
    while(cur->next != NULL && cur->next->val->get_key() < key) {
        cur = cur->next;
    }
    return cur;
}

template<typename K, typename V>
void SkipList<K,V>::delete_current_level_nodes(ListNode<K,V>* head) {
    ListNode<K,V> *cur = head;
    while(cur != NULL) {
        ListNode<K,V> *tmp = cur;
        cur = cur->next;
        delete tmp;
    }
    this->_max_level--;
    delete cur;
}

template<typename K, typename V>
void SkipList<K,V>::delete_empty_level() {
    int index = this->_max_level - 1;
    for( ; index >= 0 ; index--) {
        if(this->_head[index]->next == NULL) {
            this->delete_current_level_nodes(this->_head[index]);
            this->_head.pop_back();
        }
    }
}

template<typename K, typename V>
void SkipList<K,V>::print() {
    for(int i = this->_max_level - 1; i >= 0; i--) {
        ListNode<K,V>* cur = this->_head[i];
        while(cur != NULL && cur->next != NULL) {
            std::cout << cur->val->get_key() << "->";
            cur = cur->next;
        }
        std::cout << cur->val->get_key() << std::endl;
    }
}

template<typename K, typename V>
SkipList<K, V>::SkipList() {
    ListNode<K,V>* dummy = new ListNode<K,V>(new Node<K, V>(-1, ""));
    this->_size = 0; // 初始化结点数为0
    this->_max_level = 1; // 初始化层数为1
    this->_head.push_back(dummy); // 初始化为空结点
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
    this->_head->clear();
}

template<typename K, typename V>
void SkipList<K,V>::save_data() {
    std::ofstream _file_write;
	_file_write.open("data.txt", std::ios::out);
    ListNode<K, V> *cur = this->_head[0];
    while(cur->next != NULL) {
        _file_write << cur->next->val->get_key() << ":" << cur->next->val->get_value() << std::endl;
        cur = cur->next;
    }
    _file_write.close();
}

template<typename K, typename V>
void SkipList<K,V>::load_data() {
    std::ifstream _file_read;
	_file_read.open("data.txt", std::ios::in);
    if (!_file_read.is_open())
	{
		std::cout << "文件打开失败" << std::endl;
		return;
    }
    std::string line;
    std::string key;
    std::string value;
	while (std::getline(_file_read, line))
	{
        if(line.empty() || (line.find(delimiter) == std::string::npos)) {
            std::cout << "解析失败, 请检查格式是否正确" << std::endl;
            return;
        }
        key = line.substr(0, line.find(delimiter));
        value = line.substr(line.find(delimiter)+1, line.length());
		this->put(std::stoi(key), value);
	}
    _file_read.close();
}

template<typename K, typename V>
void get_key_value_from_string(std::string str, std::string* key, std::string* value) {
    if(str.empty() || (str.find(delimiter) == std::string::npos)) {
        return;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
std::string SkipList<K, V>::get(K key) {
    ListNode<K, V> *cur = this->find_far_right_node_in_list(key);
    if(cur->next == NULL) return "null";
    return cur->next->val->get_key() == key ? cur->next->val->get_value() : "null"; // 如果找到该key, 返回value, 没找到返回-1
}

/*
1. 当跳表为空, 插入第一个元素(pass)
2. 当跳表不为空, 末尾追加一个元素(pass)
3. 当跳表不为空, 中间追加一个元素(pass)
*/
template<typename K, typename V>
int SkipList<K,V>::put(K key, V value) {
    ListNode<K, V> *cur = find_far_right_node_in_list(key);
    if(cur->next != NULL && cur->next->val->get_key() == key) {
        return -1; // key is exist
    }
    const double PROBABILITY = 0.5;
    double num = rand()/double(RAND_MAX); //rand()%100/(double)101; 
    int level = 0; // arr[0] begin
    while(num < PROBABILITY) {
        level++;
        num = rand()/double(RAND_MAX);
    }
    // std::cout << "random level: " << level + 1<< std::endl;
    // 如果当前层数比历史层数要高, 新建层
    while(this->_max_level - 1 < level) {
        this->_head.push_back(new ListNode<K, V>(this->_head[0]->val->get_key(), this->_head[0]->val->get_value(), NULL, NULL, this->_head[this->_max_level-1]));
        this->_max_level++;
    }
    // std::cout << "current max level: "<< this->_max_level << std::endl;
    // 插入
    ListNode<K,V> *last_down = NULL;
    for(int i = 0; i <= level; i++) { 
        if(i > 0) {
            cur = find_far_right_node_in_current_level(this->_head[i], key);
        }
        // 插入首个结点
        if(cur->next == NULL) {
            cur->next = new ListNode<K,V>(key, value);
            cur->next->pre = cur;
            if(last_down != NULL) {
                cur->next->down = last_down;
            }
            last_down = cur->next;
            cur = cur->next;
            continue;
        }
        if(cur->next != NULL) { 
            // 插入中间结点
            ListNode<K,V> *tmp_next = cur->next;
            cur->next = new ListNode<K,V>(key, value);
            cur->next->pre = cur;
            if(last_down != NULL) {
                cur->next->down = last_down;
            }
            last_down = cur->next;
            cur->next->next = tmp_next;
            cur->next->next->pre = cur->next;
            cur = cur->next;
        }
    }
    this->_size++;
    return 1; // 插入成功
}

/*
1. 被删除的结点在最高层
2. 被删除的结点在中间层
3. 被删除的结点在最底层
4. 被删除的结点在末尾
5. 被删除的结点不在跳表中
*/
template<typename K, typename V>
int SkipList<K,V>::del(K key) {
    int status = -1; // 默认删除失败
    ListNode<K, V> *del_node = NULL;
    // 遍历最高层, 找到距离key最近的结点
    ListNode<K, V> *cur = find_far_right_node_in_current_level(this->_head[this->_max_level - 1], key);
    while(cur->down != NULL) {
        // 如果在上一层已经找到了key所在的结点
        if (del_node != NULL) {
            cur = del_node->down; // 来到key所在结点的下一层
            delete del_node; // 并删除上一层key所在的结点
            del_node = cur; // 标记当前层的key结点需要被删除
            cur->pre->next = cur->next; // 逻辑删除当前层的key结点
            continue;
        }
        // 如果当前结点的下一个结点的值等于key, 记录下一个结点为需要删除的结点, 并在当前层逻辑删除该结点
        if (cur->next != NULL && cur->next->val->get_key() == key) {
            status = 1; // 正在删除...
            del_node = cur->next;
            cur->next = cur->next->next;
        } else {
            // 如果当前结点的下一个结点不等于key, 来到下一层, 继续找当前层的离key最近的结点
            cur = cur->down;
            cur = find_far_right_node_in_current_level(cur, key);
        }
    }
    // 上面的循环到最底层就会退出, 只会把最底层的key逻辑删除, 实际上del_node指向的结点还没有释放
    if(del_node != NULL) {
        delete del_node;
    }
    // 假如第二层没有找到该key, 然后cur来到了第一层, 然后cur来到第一层小于key的前一个结点
    // 因为while循环是cur->down不为空才继续, 所以while循环退出, 这种情况就得单独处理
    // 判断下一个结点的值是否等于key, 如果等于key就删除, 同时把status标记为1, 表示在跳表中存在该key
    if (cur->next != NULL && cur->next->val->get_key() == key) {
        status = 1; // 正在删除...
        del_node = cur->next;
        cur->next = cur->next->next;
        delete del_node;
    }
    // 回收空层
    delete_empty_level();
    if(status == 1) {
        this->_size--; // 删除成功后跳表长度减1
    }
    return status; // 如果不存在返回-1, 删除成功返回1
}

int main() {

    srand(time(0)); // 随机种子
    
    SkipList<int, std::string> *skip = new SkipList<int, std::string>();

    // 加载已有数据
    // skip->load_data();

    // 手动插入, 插入已经存在的key会报错
    skip->put(1, "aaa");
    skip->put(2, "bbb");
    // skip->put(3, "ccc");
    skip->put(4, "ddd");
    skip->put(5, "eee");

    std::cout << "=========== 打印全部 ===========" << std::endl;
    skip->print();

    std::cout << "=========== 插入元素 ===========" << std::endl;
    skip->put(3, "test");
    skip->print();
    
    std::cout << "=========== 删除元素 ===========" << std::endl;
    skip->del(4);
    skip->print();
    
    std::cout << "=========== 获取元素 ===========" << std::endl;
    std::cout << skip->get(6) << std::endl; // 在跳表中查找key为6的结点, 不存在返回-1
    
    std::cout << "=========== 跳表长度 ===========" << std::endl;
    std::cout << skip->length() << std::endl; // 不包含头结点的个数

    // 数据持久化
    skip->save_data();
}