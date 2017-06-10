#ifndef ECMASCRIPT_BOOLEAN_H
#define ECMASCRIPT_BOOLEAN_H

class Boolean {
  bool value;

public:
  Boolean();
  Boolean(bool value);
  operator bool() const;

  bool operator==(const Boolean&) const;
  bool operator!=(const Boolean&) const;
};

#endif