# Types
### Value Types
- Can be returned from functions
- Can be passed by value or reference
- Types: `int`, 
### Reference Types
- Cannot be returned from functions (have to be passed as argument)
- Can only be passed by reference
- Types: 
# Memory Management
- Automatic memory management, without a garbage collector or reference tracker
- Memory exists only within the scope that it is created
- In order to pass memory up to a parent scope, the parent scope must define a variable and pass it into the child scope (hopefully I'll make some nice syntax for this)