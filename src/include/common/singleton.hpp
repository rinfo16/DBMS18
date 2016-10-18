#ifndef SINGLETON_HPP_
#define SINGLETON_HPP_

namespace utils {

template<class T>
class Singleton {
 public:

  static inline T & instance() {
    static T t_;
    return t_;
  }
 private:
  Singleton();
  Singleton & operator =(const Singleton &);
  Singleton(const Singleton &);
};

}  // end namespace utils

#endif // SINGLETON_HPP_
