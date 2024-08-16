# C++ pseudo-code-like language interpreter

## Requirements

1. A C++20 compatible compiler
2. (recommended) CMake 3.23 or higher

## Getting Started

1. Clone the Repository
2. Build the Project. If you're using CMake, then:
    - `cd <root_project_dir>`
    - `mkdir build`
    - `cd build`
    - `cmake ..`
    - `make`
3. Run the program:
   `./cpp_interpreter_en`

## Language Features

### Basic Information

- Everything is an expression. Statements (like [function definition](#functions)) will just return `null`
- Any valid statements or expressions will be evaluated and their result printed
- Language is whitespace-insensitive, **except** newlines, which are normally treated as the end of the statement
- Statements are separated by semicolons `;` or newlines. Using `;` is crucial when writing multiple statements in a
  single line
- Every input can be continued in the next lines, if the input line ends with backslash `\`

<details><summary>Examples</summary>

1. Using semicolons

```
> "Hello, world!";      <- ok
"Hello, world!"
```

```
> "Hello, world!"; 2 + 2 * 2      <- ok
"Hello, world!"
6
```

```
> "Hello, world!" 2 + 2 * 2      <- not ok
Error
```

2. Line continuation

```
> 2 + \
2 * 2        <- continuation
6        <- result
```

</details>

### Variables

- There is **no** variable declaration
- Variable assignment uses `:=` operator
- Variable reassignment uses `=` operator
- Multiple variables can be assigned within a single statement

<details><summary>Examples</summary>

1. Using `:=` and `=` operators

```
> x := 6      <- ok
6
> x = 12      <- ok
12
```

```
> x = 6      <- not ok
Error
```

2. Multiple variable assignment/reassignment

```
> x := y := z := 20
20
> z = y = x = "new"
"new"
```

</details>

### Data Types

- Integer (`int`)
- Float (`float`)
- String (`str`)
- Boolean (`bool`)
- List
- Dictionary
- Null

<details><summary>Details</summary>

1. Floats are created with dot `.`. Without it, they will be evaluated as integers

```
> aFloat := 3.
3.000000
> notFloat := 3
3
```

2. Integers and floats **cannot** perform arithmetic operations together.
   One of them must be explicitly cast to the type of the other first ([Type Casting](#type-casting))

```
> myInt := 8
8
> myFloat := 12.5
12.500000
> myInt + myFloat
Error
```

3. Strings can use double quotes `"` or single quotes `'`. They *should* support all valid utf-8 characters
4. Booleans are lowercase `true` and `false`
5. Lists use square brackets `[]` and can hold any data type

```
> myList := [1, 3.14, true, ["hello"], false]
[1, 3.140000, true, ["hello"], false]
```

6. Dictionaries use curly brackets `{}`, can hold only basic data type (`int`, `float`, `bool`, `str`) as a key and any
   data type as a value

```
> myDict := {"key": "val", true: ["true", 2, 3], 3.: 145}      <- ok
{"key": "val", true: ["true", 2, 3], 3.000000: 145}
```

```
> myDict := {"key": "val", ["invalid"]: "invalid"}      <- not ok
Error
```

7. Lists, dictionaries and strings can use [methods](#methods) and indexing `[]`
8. Null value can only be created through an expression returning nothing (like function definition or call)

```
> myNull := print("I return nothing")
I return nothing                         <- print
null                                     <- result
```

</details>

### Operators

- Arithmetic: `+`, `-`, `*`, `/`, `//` (integer division), `**` (exponentiation), `%` (modulo)
- Comparison: `==`, `!=`, `>`, `<`, `>=`, `<=`
- Logical: `&` (and), `|` (or), `!` (not)
- Unary: `-` (negation), `_` (absolute value), `?` ([boolean conversion](#type-casting) - works for every type)

<details><summary>Details</summary>

1. Integers and floats support all arithmetic, unary and comparison operators

```
> x := 4 * 2
8
> y := -x
-8
> _y == x
true
```

2. Strings support `+` (string concatenation), `?` and comparison operators.
   When comparing 2 strings with `==` or `!=`, their contents will be compared: otherwise, their lengths will.

```
> str1 := "Hello"
"Hello"
> str2 = str1 + ", world!"
"Hello, world!"
> str1 == str2
false
> str1 < str2
true
```

3. Logical operators work with booleans only.
4. Unlike normal type-casting, the `?` operator will **always** convert a valid expression to boolean, like so:
    - empty lists `[]` to `false`, otherwise `true`
    - empty strings `""` to `false`, otherwise `true`
    - empty dictionaries `{}` to `false`, otherwise `true`
    - `null` to `false`

```
> myList := [1, 3, 4]
[1, 3, 4]
> ?myList
true
```

```
> myDict := {}
{}
> ?myDict
false
```

</details>

### Type Casting

- Type casting uses `as` keyword
- Supported types for casting: `int`, `float`, `str`, `bool`

<details><summary>Examples</summary>

1. Type casting for binary operations

```
> myInt := 20
20
> myFloat := 14.5
14.500000
> myInt as float + myFloat       <- int cast to float
34.500000
```

```
> myFloat := 0.
0.000000
> myBool := true
true
> myBool & myFloat as bool       <- float cast to bool
false
```

```
> myStr := "Year: "
"Year: "
> myInt := 2024
2024
> myStr + myInt as str       <- int cast to str
"Year: 2024"
```

2. `as bool` vs `?` [operator](#operators)

```
> myFloat := 3.14
3.140000
> ?myFloat             <- ok
true
> myFloat as bool         <- ok
true
```

```
> myList := [1, 3, 15]
[1, 3, 15]
> ?myList                   <- ok
true
> myList as bool            <- not ok
Error
```

</details>

### Methods

- List methods: `len()`, `append()`, `remove()`, `put()`
- Dictionary methods: `size()`, `remove()`, `exists()`
- String methods: `len()`, `ltrim()`, `rtrim()`

<details><summary>Details</summary>

1. `len()` and `size()` methods don't take any arguments, they return length/size of the caller
2. `remove()` methods take an index/key of the element to be removed as an argument
3. `append()` method takes 1 argument that will be added to the end of the list
4. `put()` takes an index as its 1st argument, and a value to be put at that index as its 2nd

```
> myList := [1, 2, 3, 4, 5]
[1, 2, 3, 4, 5]
> myList.put(2, 2.5)
[1, 2, 2.5, 3, 4, 5]
```

5. `exists()` method takes a key as an argument and returns true if that key exists, false otherwise
6. `ltrim()` and `rtrim()` methods as an argument take a string of characters that will be removed starting from their
   respected side (r: right, l: left) until a different character is found.
   The order of the characters doesn't matter

```
> lStr := "Hhhhello, world!"
"Hhhhello, world!"
> lStr.ltrim("Hh")
"ello, world!"
```

```
> rStr := "Hello, world! aaaabbcde"
"Hello, world! aaaabbcde"
> rStr.rtrim("abce d")
"Hello, world!"
```

</details>

### Control Structures

- If-else: `if condition then ... [else ...] stop`
- For loop: `for i in n..m[:s] do ... stop` or `for key in dict do ... stop`
- While loop: `while condition do ... stop`

<details><summary>Details</summary>

1. All control structures can be continued in the next lines after their starting keyword, without the use of `\`
   The input will stop after the outermost `stop` keyword was passed

```
> if true then       <- continues
    x := 5
stop                 <- stops
```

```
> if true then        <- outer block
    x := 5
    if false then        <- inner block
        x = x + 5
    else
        x = x - 5
    stop               <- inner stop
stop                <- outer stop
```

2. All control structures can be passed in a single line too

```
> x := if 1 > 0 then 15 else "fifteen" stop         <- the last evaluated expression is 15
15                                           <- the value assigned to x
```

3. For loops can iterate over range, as well as over dictionary's keys.
   When using range-based loop, the step taken after every iteration can be specified with `:`. The default step is 1

```
> sum := 0
0                      <- before
> for i in 1..6 do         <- step = 1
    sum = sum + i
stop
21                    <- after
```

```
> sum := 0
0                        <- before
> for i in 1..6:2 do         <- step = 2
    sum = sum + i
stop
9                      <- after
```

```
> myDict := {"one": 1, "two": 2, "three": 3}
{"one": 1, "two": 2, "three": 3}                  <- before
> for key in myDict do
    myDict[key] = 0
stop
> myDict
{"one": 0, "two": 0, "three": 0}               <- after
```

4. For loops cannot iterate over lists directly, however the same behaviour can be achieved
   with `len()` [method](#methods)

```
> myList := [1, -2, 3, -4, 5]
[1, -2, 3, -4, 5]                       <- before
> for i in 0..myList.len()-1 do
   myList[i] = myList[i] + 1
stop
> myList
[2, -1, 4, -3, 6]                      <- after
```

5. While loop's maximum number of iterations is 99999.

</details>

### Functions

- Function definition: `def function_name(parameters) as ... stop`
- Function call: `function_name(arguments)`

<details><summary>Details</summary>

1. Function definitions always return `null`
2. Functions can use `return` keyword to explicitly return a value.
   Without it, the last evaluated expression will be returned

```
> def addTwo(x) as
   if (x < 0) then return 0 stop           <- the function will explicitly return 0
   x + 2                                 <- or implicitly x + 2
stop
> addTwo(-1)
0
> addTwo(1)
3
```

3. When trying to return `null` with `return`, the `return` keyword must be followed by either newline or `;`

```
> def returnIfTwo(x) as
   if x != 2 then return; stop          <- ok
   x
stop
> returnIfTwo(0)
null
> returnIfTwo(2)
2
```

```
> def returnIfTwo(x) as
   if x != 2 then return stop           <- not ok
   x
stop
Error
```

4. Functions cannot be defined with built-in function's name

```
> def print() as return 20 stop
Error
```

5. Function definitions can use variable-length arguments with the use of `..`.
   That parameter is a list of all passed arguments to a function call

```
> def sum(..args) as
   sum := 0.
   for i in 0..args.len()-1 do           <- iterating over 'args' list
      sum = sum + args[i] as float
   stop
   sum
stop
> sum(2, 4)                           <- ok
6
> sum(13, -2, 9, 3.14, 80.5)             <- ok too
103.640000
```

</details>

### Built-in Functions

- `print()`: Print values seperated by `,` to stdout. Type-casting to string is not required
- `type()`: Get the type of a value as a string
- `roundf()`: Round a float to given precision
- `round()`: Round a float to the nearest integer
- `floor()`: Round a float down to the nearest integer
- `ceil()`: Round a float up to the nearest integer

<details><summary>Examples</summary>

1. Using `type()`

```
> myFloat := 3.14
3.140000
> def printInt(i) as
   if type(i) != "int" then
      print("Not an int!")
      return
   stop
   print(i)
stop
> printInt(myFloat)
Not an int!
```

2. Using `roundf()`

```
> myFloat := 3.141592
3.141592
> roundf(myFloat, 2)
3.140000
```

</details>
