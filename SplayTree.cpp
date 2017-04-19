#include <utility>

template <typename ValueType>
class Set {
 private:
    struct Node {
        Node *left;
        Node *right;
        Node *parent;
        ValueType value;

        Node(Node *left_, Node *right_, Node *parent_, ValueType value_) :
            left(left_), right(right_), parent(parent_), value(value_) {}

        Node(ValueType value_) : value(value_) {}
    };

 public:
    Set() : root(nullptr), node_number(0) {}

    template <typename Iterator>
    Set(Iterator start, Iterator end) {
        root = nullptr;
        node_number = 0;
        for (auto it = start; it != end; ++it) {
            insert(*it);
            if (most_left == nullptr || *it < most_left->value) {
                most_left = root;
            }
            if (most_right == nullptr || most_right->value < *it) {
                most_right = root;
            }
        }
    }

    Set(std::initializer_list<ValueType> list) : Set(list.begin(), list.end()) {}

    Node* dfsCreate(Node* vertex) {
        if (vertex == nullptr) {
            return nullptr;
        }
        Node* node = new Node(vertex->value);
        if (most_left == nullptr || node->value < most_left->value) {
            most_left = node;
        }
        if (most_right == nullptr || most_right->value < node->value) {
            most_right = node;
        }
        node->left = dfsCreate(vertex->left);
        node->right = dfsCreate(vertex->right);
        if (node->left != nullptr) {
            node->left->parent = node;
        }
        if (node->right != nullptr) {
            node->right->parent = node;
        }
        return node;
    }
    Set(const Set<ValueType> & set) {
        root = dfsCreate(set.root);
        if (root != nullptr) {
            root->parent = nullptr;
        }
        node_number = set.node_number;
    }

    void dfsDel(Node* node) {
        if (node == nullptr) {
            return;
        }
        dfsDel(node->left);
        dfsDel(node->right);
        delete node;
    }

    Set & operator=(const Set<ValueType> & set) {
        if (this != &set) {
            dfsDel(root);
            root = nullptr;
            most_left = nullptr;
            most_right = nullptr;
            node_number = 0;
            root = dfsCreate(set.root);
            if (root != nullptr) {
                root->parent = nullptr;
            }
            node_number = set.node_number;
        }
        return *this;
    }

    ~Set() {
        dfsDel(root);
    }

    std::size_t size() const {
        return node_number;
    }

    bool empty() const {
        return node_number == 0;
    }

    inline void rightRotate(Node *node) const {
        Node *left = node->left;
        Node *left_right = left->right;
        Node *parent = node->parent;
        if (parent != nullptr) {
            if (parent->right == node)
                parent->right = left;
            else
                parent->left = left;
        }
        if (left_right != nullptr)
            left_right->parent = node;
        left->parent = parent;
        left->right = node;

        node->parent = left;
        node->left = left_right;
    }
    inline void leftRotate(Node *node) const {
        Node *right = node->right;
        Node *right_left = right->left;
        Node *parent = node->parent;
        if (parent != nullptr) {
            if (parent->right == node)
                parent->right = right;
            else
                parent->left = right;
        }
        if (right_left != nullptr)
            right_left->parent = node;
        right->parent = parent;
        right->left = node;

        node->parent = right;
        node->right = right_left;
    }

    void splay(Node *node) const {
        for (Node* current_ansestor = node->parent;
            current_ansestor != nullptr; current_ansestor = node->parent) {
            Node *g_parent = current_ansestor->parent;
            if (g_parent == nullptr) {
                if (current_ansestor->left == node)
                    rightRotate(current_ansestor);
                else
                    leftRotate(current_ansestor);
                break;
            }
            if (g_parent->left == current_ansestor) {
                if (current_ansestor->left == node) {
                    rightRotate(g_parent);
                    rightRotate(current_ansestor);
                } else {
                    leftRotate(current_ansestor);
                    rightRotate(g_parent);
                }
            } else {
                if (current_ansestor->left == node) {
                    rightRotate(current_ansestor);
                    leftRotate(g_parent);
                } else {
                    leftRotate(g_parent);
                    leftRotate(current_ansestor);
                }
            }
        }
        root = node;
    }
    void insert(ValueType value) {
        if (root == nullptr) {
            root = new Node(nullptr, nullptr, nullptr, value);
            most_left = root;
            most_right = root;
            ++node_number;
            return;
        }
        Node *current = root;
        for (current = root; (current->value < value) || (value < current->value); ) {
            if (value < current->value) {
                if (current->left) {
                    current = current->left;
                } else {
                    current->left = new Node(nullptr, nullptr, current, value);
                    current = current->left;
                    if (most_left == nullptr || current->value < most_left->value) {
                        most_left = current;
                    }
                    if (most_right == nullptr || most_right->value < current->value) {
                        most_right = current;
                    }
                    ++node_number;
                    break;
                }
            } else {
                if (current->right) {
                    current = current->right;
                } else {
                    current->right = new Node(nullptr, nullptr, current, value);
                    current = current->right;
                    if (most_left == nullptr || current->value < most_left->value) {
                        most_left = current;
                    }
                    if (most_right == nullptr || most_right->value < current->value) {
                        most_right = current;
                    }
                    ++node_number;
                    break;
                }
            }
        }
        splay(current);
    }

    class iterator {
     public:
        iterator() : _point(nullptr), _flag(false) {}
        iterator(Node* point_, bool flag_ = false) : _point(point_), _flag(flag_) {}
        ~iterator() {}
        Node* left_father(Node* from, Node* vertex) const {
            if (vertex == nullptr) {
                return nullptr;
            }
            if (vertex->left == from) {
                return vertex;
            }
            return left_father(vertex, vertex->parent);
        }
        iterator& operator++() {
            if (_point->right != nullptr) {
                _point = _point->right;
                while (_point->left != nullptr) {
                    _point = _point->left;
                }
            } else {
                Node* node = left_father(_point, _point->parent);
                if (node != nullptr) {
                    _point = node;
                } else {
                    _flag = true;
                }
            }
            return *this;
        }
        iterator operator++(int) {
            iterator tmp(*this);
            operator++();
            return tmp;
        }

        Node* right_father(Node* from, Node* vertex) const {
            if (vertex == nullptr) {
                return nullptr;
            }
            if (vertex->right == from) {
                return vertex;
            }
            return right_father(vertex, vertex->parent);
        }
        iterator& operator--() {
            if (_flag) {
                _flag = false;
                return *this;
            }
            if (_point->left != nullptr) {
                _point = _point->left;
                while (_point->right != nullptr) {
                    _point = _point->right;
                }
            } else {
                _point = right_father(_point, _point->parent);
            }
            return *this;
        }
        iterator& operator--(int) {
            iterator tmp(*this);
            operator--();
            return tmp;
        }

        bool operator==(const iterator& rhs) const {
            return (_point == rhs._point && _flag == rhs._flag);
        }

        bool operator!=(const iterator& rhs) const {
            return (_point != rhs._point || _flag != rhs._flag);
        }

        const ValueType& operator*() const {
            return _point->value;
        }
        ValueType* operator->() const {
            return &(_point->value);
        }
        Node*& point() {
            return _point;
        }
        Node* point() const {
            return _point;
        }
        bool flag() const {
            return _flag;
        }

     private:
        Node* _point;
        bool _flag;
    };

    iterator begin() const {
        return most_left;
    }

    iterator end() const {
        if (root == nullptr) {
            return nullptr;
        }
        return iterator(most_right, true);
    }

    iterator find(ValueType value) const {
        if (root == nullptr)
            return end();
        Node *current = root;
        while (current != nullptr) {
            if (!(current->value < value) && !(value < current->value))
                break;
            if (value < current->value) {
                if (current->left)
                    current = current->left;
                else
                    break;
            } else {
                if (current->right)
                    current = current->right;
                else
                    break;
            }
        }
        splay(current);
        if (!(current->value < value) && !(value < current->value))
            return current;
        else
            return end();
    }
    Node* bigger_vertex(Node* vertex, ValueType value) const {
        if (vertex == nullptr || (!(vertex->value < value) && !(value < vertex->value))) {
            return vertex;
        }
        if (vertex->value < value) {
            return bigger_vertex(vertex->right, value);
        }
        Node* node = bigger_vertex(vertex->left, value);
        if (node != nullptr) {
            return node;
        } else {
            return vertex;
        }
    }
    iterator lower_bound(ValueType value) const {
        if (root == nullptr || most_right->value < value) {
            return end();
        } else {
            Node *vertex = bigger_vertex(root, value);
            splay(vertex);
            return vertex;
        }
    }
    void erase(ValueType value) {
        auto it = find(value);
        if (it.point() == nullptr || it.flag())
            return;
        Node *node = (it).point();
        splay(node);
        --node_number;
        if (node_number == 0) {
            delete node;
            root = nullptr;
            most_left = nullptr;
            most_right = nullptr;
            return;
        }
        if (most_left == node) {
            most_left = (++(iterator(node))).point();
        } else {
            if (most_right == node) {
                most_right = (++(iterator(node))).point();
            }
        }
        Node *left = node->left;
        if (left == nullptr) {
            root = node->right;
            root->parent = nullptr;
            delete node;
            return;
        }
        while (left->right != nullptr)
            left = left->right;
        if (node->right != nullptr) {
            left->right = node->right;
            node->right->parent = left;
        }
        root = node->left;
        root->parent = nullptr;
        delete node;
    }

 private:
    mutable Node *root;
    mutable Node *most_left = nullptr;
    mutable Node *most_right = nullptr;
    std::size_t node_number;
};


