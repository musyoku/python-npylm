/**
*This project was developed as a part of the course ADS 2 at Charles University in Prague.
*@author Lukas Jelinek lukas.jelinek1@gmail.com
* \version 1.0.0
* \date 22.1.2015
* \copyright MIT License http://opensource.org/licenses/MIT
*/
#ifndef SPLAY_TREE
#define SPLAY_TREE
#include <memory>
#include <ostream>
#include <vector>
namespace pl{
    template <typename K,class V>
    class SplayTree
  {
   
   /**
   * Standard binary search tree 
   *        parent
   *     |           |
   *  left node     right node
   * first == key of the node
   * second == value of the node
   */
    
   struct node {
       node():parent(nullptr){}
       std::unique_ptr<node> left, right;
       node * parent;
       K first;
       V second;
   };
   typedef typename std::unique_ptr<node> node_ptr;
   
   //**********************************************************************************
   // implementation of  iterators for the SplayTree as a iner class in SplayTree
   //**********************************************************************************
   class iterator{
       public:
           typedef std::forward_iterator_tag iterator_category;
           typedef int difference_type;
           iterator() : ptr_(nullptr){ }
          explicit iterator(node * ptr) : ptr_(ptr) { }
           iterator(iterator& it){ ptr_ = it.ptr_;}
           iterator& operator=(iterator& it){
               if (this == &it) return *this;
               ptr_ = it.ptr_;
               return *this;
           }
           iterator& operator++() {
               node * prev= ptr_; 
               if(ptr_ != nullptr){ // if i==nullptr we are in root or tree is empty
                   if (ptr_->right != nullptr) {
                       while (ptr_->left) ptr_ = (ptr_->left).get();
                   }else{
                       ptr_ = ptr_->parent;
                       while (ptr_->right.get() == prev && ptr_ != nullptr){
                           prev = ptr_;
                           ptr_ = ptr_->parent;
                       }
                   }
               }
               return *this; 
           }
           iterator& operator++(int junk) { ++(*this); }
           node & operator*() { return *ptr_; }
           node * operator->() { return ptr_; }
           bool operator==(const iterator & rhs) { return ptr_ == rhs.ptr_; }
           bool operator!=(const iterator & rhs) { return ptr_ != rhs.ptr_; }
       private:
           node * ptr_;
           
       };

       class const_iterator{
       public:
           const_iterator() : ptr_(nullptr) { }
           const_iterator(node * ptr) : ptr_(ptr) { }
           const_iterator(const const_iterator& it){ ptr_ = it.ptr_; }
          const const_iterator& operator=(const const_iterator& it){
               if (this == &it) return *this;
               ptr_ = it.ptr_;
               return *this;
           }
          const const_iterator& operator++() { 
               node * prev = ptr_;
               if (ptr_ != nullptr){ // if i==nullptr we are in root or tree is empty
                   if (ptr_->right != nullptr) {
                       ptr_ = ptr_->right.get();
                       while (ptr_->left) ptr_ = (ptr_->left).get(); // go to min of right subtree at root
                   }
                   else{
                       ptr_ = ptr_->parent;
                       while (ptr_ != nullptr && ptr_->right.get() == prev){
                           prev = ptr_;
                           ptr_ = ptr_->parent;
                       }
                   }
               }
               return *this;
           }
           const const_iterator& operator++(const int junk) { ++(*this); }
           const node & operator*() { return *ptr_; }
           const node * operator->() { return ptr_; }
           bool operator==(const  const_iterator& rhs) { return ptr_ == rhs.ptr_; }
           bool operator!=(const  const_iterator& rhs) { return ptr_ != rhs.ptr_; }
       private:
           node * ptr_;
       };
    //**********************************************************************************
    // public part of SplayTree class
    //**********************************************************************************  
   public:
      SplayTree() :node_count_{ 0 } {}
      ~SplayTree() {}
      /**
      * Inserts element(s) into the container, if the container doesn't already contain an element with an equivalent key. 
      * If it contains the same key old value in node is replaced by new value.
      * @param key the key used for sorting in tree
      * @param val Value stored in the node
      */
      void insert(const K & key,const V & val);
      /**
      * Finds an element with key equivalent to key. 
      * @return Iterator to an element with key equivalent to key. If no such element is found,
      *         past-the-end (see end()) iterator is returned. 
      */
      iterator find(const K & key){ return find_(key)?iterator(find_(key)):end();}
      /**
      * Finds an element with key equivalent to key.
      * @return Const_iterator to an element with key equivalent to key. If no such element is found,
      *         past-the-end (see cend()) const_iterator is returned.
      */
      const_iterator find(const K & key)const{ return find_(key) ? const_iterator(find_(key)) : cend(); }
      /**
      *  Removes element with the key value key.
      *@param key key value of the elemet to remove
      */
      void erase(const K & key){ remove(find_(key)); }
      /**
      *  Removes the element at pos.
      *@param pos iterator on the element at the pos
      */
      void erase(iterator pos){ remove(pos.operator->()); }
      /**
      * Outputs DOT instructions to draw whole tree.
      *@param o output streem for DOT instructions
      */
      void get_dot(std::ostream & o){ getDOT(o); }
        /**
        * Removes all elements from the container. 
        * Invalidates any references, pointers, or iterators referring to contained elements. 
        * May invalidate any past-the-end iterators.
        */
        void clear() { purgeTree(root_); }
        /**
        * Checks if the container has no elements.
        * @return true if container is empty
        */
        bool empty() const { return node_count_ == 0 ? true : false; }
        /**
        * Returns the number of elements in the container.
        * @return number of elements in the tree
        */
        size_t size() const { return node_count_; }
        /**
        *Returns an iterator to the first element of the container. 
        *If the container is empty, the returned iterator will be equal to end(). 
        */
        iterator begin() { return node_count_==0? end():iterator(subtreeMin(root_.get())); }
        /**
        *Returns an iterator behind the last element of the container.
        */
        iterator end() { return iterator(); }
        /**
        *Returns a const_iterator to the first element of the container.
        *If the container is empty, the returned iterator will be equal to cend().
        */
        const_iterator cbegin() const { return node_count_==0? cend():const_iterator(subtreeMin(root_.get())); }
        /**
        *Returns an const_iterator behind the last element of the container.
        */
        const_iterator cend() const { return  const_iterator(); }
        /**
         *Exchanges the contents of the container with those of other. 
         *Does not invoke any move, copy, or swap operations on individual elements. 
         *All iterators and references become invalidated. 
         */
        void swap(SplayTree & other){ node_ptr temp = move(root_); root_ = move(other.root_); other.root_ = move(temp); }

   private:
       size_t node_count_;
       node_ptr root_;
       void leftRotate(node * x);
       void rightRotate(node * x);
       void splay(node * x);
       node * subtreeMax(node* p) const; // find max in subtree
       node * subtreeMin(node* p) const; // find min in subtree
       node* find_(const K & key);
       node* find_(const K & key)const{ return find_(key); }
       void purgeTree(node_ptr & n);
       void getDOT(std::ostream &o);
       void remove(node * n);

  };
  //**********************************************************************************
  // Implementation of SplayTree class
  //**********************************************************************************  
  template <typename K, typename V>
  void SplayTree<K,V>::leftRotate(node * rt)
  {
      node* pivot = rt->right.get();
      node_ptr pv = move(rt->right);
      node * grand_pa = rt->parent;
           
      if (pv->left){ //left node of pv to right node of rt
           rt->right = move(pv->left);
           rt->right->parent = rt;
       }
       if (!grand_pa){ // if rt is really root of the tree
           pv->left = move(root_); 
           root_ = move(pv);
           pivot->parent=nullptr;
           rt->parent = pivot;
       }else{
          if (grand_pa->right.get() == rt){ // rt is in right branch from his grandparent
               pv->left = move(grand_pa->right);
               grand_pa->right = move(pv);
               pivot->parent = grand_pa;
               rt->parent = pivot;

           }else{ // rt is in left branch from his grandparent
               pv->left = move(grand_pa->left);
               grand_pa->left = move(pv);
               pivot->parent = grand_pa;
               rt->parent = pivot;
           }
       }
      
      
  }
  template <typename K, typename V>
  void SplayTree<K,V>::rightRotate(node * rt)
  {     
      node* pivot = rt->left.get();
      node_ptr pv = move(rt->left);
      node * grand_pa= rt->parent;
      if (pv->right){ //right node of pv to left node of rt
          rt->left = move(pv->right);
          rt->left->parent = rt;
      }
      if (!grand_pa){ // if rt is really root of the tree
          pv->right = move(root_);
          root_ = move(pv);
          pivot->parent = nullptr;
          rt->parent = pivot;
      }
      else{
          if (grand_pa->right.get() == rt){ // rt is in right branch from his grandparent
              pv->right = move(grand_pa->right);
              grand_pa->right = move(pv);
              pivot->parent = grand_pa;
              rt->parent = pivot;

          }
          else{ // rt is in left branch from his grandparent
              pv->right = move(grand_pa->left);
              grand_pa->left = move(pv);
              pivot->parent = grand_pa;
              rt->parent = pivot;
          }
      }

  }
  template <typename K, typename V>
  void SplayTree<K,V>::splay(node * x)
  {
      if (x == nullptr)return;
      while (x->parent)
      {          
          if (!x->parent->parent) // zig step
          {
              if (x == x->parent->left.get())rightRotate(x->parent);
              else leftRotate(x->parent);
          }
          else if (x == x->parent->left.get() && x->parent == x->parent->parent->left.get()) //zig zig step left
          {
              rightRotate(x->parent->parent);
              rightRotate(x->parent);
          }
          else if (x == x->parent->right.get() && x->parent == x->parent->parent->right.get()) // zig zig step right
          {
              leftRotate(x->parent->parent);
              leftRotate(x->parent);
          }
          else if (x == x->parent->left.get() && x->parent == x->parent->parent->right.get()) //zig zag rotation left
          {
              rightRotate(x->parent);
              leftRotate(x->parent);
          }
          else // zig zag rotation righ
          {
              leftRotate(x->parent);
              rightRotate(x->parent);
          }
      }

  }
  template <typename K, typename V>
typename  SplayTree<K,V>::node* SplayTree<K, V>::subtreeMax(node * p) const // find max in subtree
  {
     node * t = p;
      while (t->right) t=(t->right).get();
      return t;
  }
  template <typename K, typename V>
 typename SplayTree<K,V>::node*  SplayTree<K, V>::subtreeMin(node * p) const // find min in subtree
  {
      node * t = p;
      while (t->left) t = t->left.get();
      return t;
  }
  template <typename K, typename V>
  void SplayTree<K, V>::purgeTree(node_ptr & n)
  {
      n.release();
      
  }


  template <typename K, typename V>
  void SplayTree<K, V>::insert(const K & key, const V & val)
    {
        node *p = root_.get();
        node *prev = nullptr;
        while (p)
        {
            prev = p;
            if (key == p->first) // key already exist in tree
                break;
            else if (key < p->first) // key is smaller than current node
                p = p->left.get();
            else if (key > p->first)
                p = p->right.get();

        }
        // Program is in the leaf of node
        node_ptr n = std::unique_ptr<node>(node());//std::make_unique<node>();
        n->first = key;
        n->second = val;
        ++node_count_;
        /*if (p != nullptr){ // p is pointing on node with same key as n
            if (prev->right.get() == p){
                //prev.right ---> p  ->   prev.right --->n.right ---> p
                n->right = move(prev->right);
                prev->right = move(n);
                prev->right->parent = prev;
                p->parent = prev->right.get();
                splay(prev->right.get());
            }
            else{
                n->left = move(prev->left);
                prev->left = move(n);
                prev->left->parent = prev;
                p->parent = prev->left.get();
                splay(prev->left.get());
            }
        }
        else{*/
            if (prev == nullptr) root_ = move(n); // if empty tree
            else if (prev->right){ // adding new node to left branch under prev*
                prev->left = move(n);
                prev->left->parent = prev;
                splay(prev->left.get());
            }
            else{ // // adding new node to right branch under prev*
                prev->right = move(n);
                prev->right->parent = prev;
                splay(prev->right.get());
            }
        //}
       
        
        
    }
    template <typename K, typename V>
typename     SplayTree<K, V>::node * SplayTree<K,V>::find_(const K& key)
    {
        node *p = root_.get();
        while (p)
        {
            if (key < p->first) p = p->left.get();
            else if (key > p->first) p = p->right.get();
            else {
                splay(p);
                return root_.get();
            }
        }
        return nullptr;

    }
    template <typename K, typename V>
    void SplayTree<K, V>::remove(node* n)
    {
        //node* n = find_(key);
        if (n == nullptr)return;
        --node_count_;
        if (!n->left && !n->right){// just root
            root_.release();
            return;
        } else  if (!n->left){  //nothing to the left from the root n
            root_ = move(n->right);
            root_->parent = nullptr;
        }else if (!n->right){ //nothing to the right from the root n
            root_ = move(n->left);
            root_->parent = nullptr;
        }else
        {
            node * d = subtreeMax(n->left.get());
            node * d_min;
            node_ptr left;
            if (d->left){
                d_min = subtreeMin(d->left.get());
                left= move(root_->left);
                d->right = move(root_->right); // connect right side to local max of left tree
                d->right->parent = d;
                root_ = move(d->parent->right); // lokal max of left tree (d) conect to root
                root_->parent = nullptr;
                d_min->left = move(left); // conect rest of the left branch under local min under d
                d_min->left->parent = d_min;
            }else{ // just one node on the left side
                d->right = move(root_->right); // connect right side to local max of left tree
                d->right->parent = d;
                root_ = move(root_->left); // lokal max of left tree (d) conect to root
                root_->parent = nullptr;
            }
           
                      
        }
     
       
    }
    template <typename K, typename V>
    void SplayTree<K, V>::getDOT(std::ostream & o){
        o << "digraph G{ graph[ordering = ""out""]; " << std::endl;
        if (node_count_ == 1)
            o << root_->first << ";" << std::endl;
        else{
            for (auto it = cbegin(); it != cend(); ++it){
                if (it->parent)
                    o << it->parent->first << " -> " << it->first << ";" << std::endl;
            }
        }
        o << "}" << std::endl;
    }


}// end of pl


#endif