
# Onde estamos

Estamos fazendo apenas a tokenização, após esse momento analizamos se no token
tem ou não o par de aspas. Se tiver vamos para função que explode as aspas, se
não tiver vamos para função que abre o here_doc.

- **exemplo:**

"ls -a -> abre here_doc -> recebe restante do comando até as aspas ->
substitui o token ("ls -a) por ("ls -a o_que_vem_do_here_doc") e provalvemente
vai dar ruim.

# Não esquecer que quando arrays de strings começarem com aspas que não são fechadasm, as words são contadas como uma!!

Lembrar de que quando tiver DOIS PIPES um atras do outro seve-se retornar 0 na sintax_analysis.c

# Funções autorizadas

| Função | resumo | argumentos | retorno |
| :----: | :----- |  :------:  | :----:  |
| `rl_clear_history` | Clear the history list by deleting all of the entries | *void* | *void* |
| `rl_on_new_line` | Tell the *update function* that we have moved onto a new line | *void* | *`int`* |
| `rl_replace_line` | Replace the contents of *`rl_line_buffer`* with *text* | `cosnt char *text` new content line. `int clear_undo` if *clear_undo* id non-zero, the undo list associated with the current line is cleared | *void* |
| `rl_redisplay` | chage what's displayed on the screen to reflect the current contents of *rl_line_buffer* | *void* | *void* |
| `add_history` | add one line to history | `char *line` line to be saved | *void* |
| `wait` | suspends execution of the calling thread until one of its children terminates | `int *wstatus` | `pid_t` |
| `waitpid` | suspends execution of the calling thread until a child specified by *pid* argument has change state | `pid_t pid`, `int *wstatus`, `int options` | `pid_t` |
| `wait3` | is similar to *waitpid*, but sdditionally return resource usage information about the child | `int *wstatus`, `int options`, `struct rusage *rusage` | `pid_t` |
| `wait4` | is similar to *waitpid*, but sdditionally return resource usage information about the child | `pid_t pid`, `int *wstatus`, `int options`, `struct rusage *rusage` | `pid_t` |
| signal |      |||
| sigaction |   |||
| sigemptyset | |||
| sigaddset |   |||
| kill |        |||
| getcwd |      |||
| chdir |       |||
| stat |        |||
| lstat |       |||
| fstat |       |||
| unlink |      |||
| dup |         |||
| dup2 |        |||
| opendir |     |||
| readdir |     |||
| strerror |    |||
| perror |      |||
| isatty |      |||
| ttyname |     |||
| ttyslot |     |||
| ioctl |      |||
| getenv |      |||
| tcsetattr |   |||
| tcgetattr |   |||
| tgetent |     |||
| tgetflag |    |||
| tgetnum |     |||
| tgetstr |     |||
| tgoto |       |||
| tputs |       |||