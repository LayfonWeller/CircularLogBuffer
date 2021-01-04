# Circular Logs Buffer

A small C++ templated object that contains 0 to SIZE (template parameter) object.

It's akin to a circular buffer, but reading counter is not present. It keeps the last SIZE element.
When all spot are filled, it will instead replace the oldest element.

It was design to run on embedded system and can run on cortex-m4 without issues.


# Tests
To be done