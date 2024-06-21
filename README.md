## Simple Shell
This project is about creating a C program that functions as a basic Unix/Linux
(*nix) shell. 

The core functionalities of this project include Prompt and Command Entry, Handling Command
Line Arguments, Arbitrary Command Length Acceptance, Command Parsing, Built-in Exit Command, and
Memory Management.

The program also expand the functionality to explore information from the /proc
filesystem on Linux, presenting it to the user in a structured manner.

Lastly, the program also includes a history command. When executed it displays the last ten 
commands entered by the user.

While this project focuses on fundamental shell features, it
does not include the extensive functionality found in advanced shells like Bash. 

## How to Use
1. Clone the Repository:
    ```code
    git clone git@github.com:lc37819/Simple-Shell.git
2. Navigate to the Directory:
    ```code
    cd Simple-Shell
3. Build the Project:
    ```code
    make
4. Run the Shell:
   ```code
   ./simple_shell
## Documentation

## main()
This was the first function that was implemented. I added (void)argv; to bypass the warning 
that was emitted by the compiler when the code was compiled. This is because argv is not 
used in my implementation.

Next I check and validate the number of arguments when the executable file is being 
executed. If more than one argument is passed, an error message is displayed and the 
program exits.

Lastly, user_propt_loop is called.

## user_propt_loop()
This function is used to continually prompt the user for input. Initially, a prompt symbol 
is displayed and when the user enters a command it is stored and saved to history, this 
happens until the user enters exit with an appropriate exit code status. Depending to what 
the user enter one of a list of built in commands or programs are executed.

1. proc - displays content for the proc file system

2. exit - exits the program

3. history - displays the last ten commands entered by the user

4. executable programs like ls, ls -l, cat, etc...

Once the user is done using the program and enters exit all memory is freed and the program 
is terminated.

## get_user_command()
This function is responsible for reading user input and returning it as a string. Memory is 
allocated to accept an arbitrary command length, this function include dynamic resizing 
therefore when the buffer is full, more space is created. Once the command has been read 
and validation has been done, the dynamically allocated data is freed.

## parse_command()
This function is used for parsing the user input and splitting it into individual arguments.
args is used as an array of arguments and accepts an arbitrary number of arguments. This is 
done incrementally using the next_segment helper function. Once the array reaches it's 
capacity it is dynamically resized. Furthermore, the function ignore white space, stores 
the user entered arguments in an array and is used for further processing by other parts of 
the shell, i.e. proc_command, exit_commmand, execute_command. etc...

## exit_command()
This function is used for terminating the shell. It also allows a user to specify an exit 
status. This function accepts either zero or one argument, validation is done to ensure
this. If an argument is passed it is checked by the parse_integer helper function. If the 
parsing fails, the command will be ignored, and the shell prompts for another command. If 
the parsing succeeds, the shell should exit with the parsed integer as the exit status.

## proc_command()
This function is used for reading and displaying content from files within the /proc 
directory of a Linux system. Initial the number of arguments is checked. This function 
requires at least one argument. Based on the argument passed the file is read and displayed 
to the user. If an error occurs while opening the file it is displayed to the user. Once 
that is done allocated memory is freed and the file is closed. 

## execute_command()
This function is used for executing commands inputted by the user, apart from the built in 
commands proc, exit, and history. It creates a child process using fork(), the child 
process executes the user entered command using execvp(), if it fails memory is freed and 
the program exits. The parent process waits for the child process to complete.

## history_command()
This function is responsible for managing and displaying the history of user commands. It 
reads the last commands saved in a file located in the user's home directory. The 
save_command is used to save all commands enter by the user in the shell. Once the user 
enters a command it is saved to the .421sh hidden file in the user's home directory. 
is_blank is used to check if the user enters a blank command. If the user enters a command 
with no actual text it is not saved. Once the last ten commands are displayed all 
dynamically allocated data is freed and the file is closed.
