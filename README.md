
# Onde estamos

Estamos fazendo apenas a tokenização, após esse momento analizamos se no token
tem ou não o par de aspas. Se tiver vamos para função que explode as aspas, se
não tiver vamos para função que abre o here_doc.

- **exemplo:**

"ls -a -> abre here_doc -> recebe restante do comando até as aspas ->
substitui o token ("ls -a) por ("ls -a o_que_vem_do_here_doc") e provalvemente
vai dar ruim.

- *tests to do*

- [ ] ls | grep 'Libft'
- [ ] ls | grep Libft | tr f \"

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
| `signal` | sets the disposition of the signal *signum* to *handler* | `int signum`, `sighandler_t handler` | returns the previus value of the signal handler of *SIG_ERR* on error |
| `sigaction` | system call is used to change the action taken by a process on receipt of a specific signal | `int signum`, `const struct sigaction *act`, `struct sigaction *oldact` | `0` on success, `-1` on error |
| `sigemptyset` | allow the manipulation of POSIX signal set. Initializes the signal set given by *set* to empty, with all signals excluded from the set | `sigset_t *set` | `0` on success, `-1` on error |
| `sigaddset` | add signal *signum* from *set* | `sigset *set`, `int signum` | `0` on success, `-1` on error |
| `kill` | system call can be used to send any signal to any process group or process | `pid_t pid`, `int sig` | `0` on success, `-1` on error and *errno* is set appropriately |
| `getcwd` | copies an absolute pathname of te current working directory to the array pointed to by *buf*, which is of length *size* | `char *buf`,`size_t size` | on success return a poiter ro a string containing the pathname of the current working directory. on failure return NULL, and *errno* is set to indicate the error |
| `chdir` | changes the current working directory of the calling process to the dir specified in *path* | `const char *path` | `0` on success or `-1` on error and errno is set appropriately |
| `stat` | return information about a file, in the buffer pointed to by *statbuf*. Permission is required on all of the directorries in *pathname* that lead to the file | `const char *pathname`, `struct stat *statbuf` | `0` on success or `-1` on error and errno is set appropriately |
| `lstat` | is identical to `stat`, except that if *pathname* is a symbolic lin, then id returns infomarion about the link itself, not the file that it refers to. | `const char *pathname`, `struct stat *statbuf` | `0` on success or `-1` on error and errno is set appropriately |
| `fstat` | is identical to `stat`, except that the file about which information is to be retrieved is specified by the file descriptor fd. | `int fd`, `struct stat *statbuf` | `0` on success or `-1` on error and errno is set appropriately |
| `unlink` | deletes a name from the filesystem. If that name was the last link to a file and no processes have the file open, the file is deleted and the space it was using is made available for reuse. If the name was the last link to a file but any processes still have the file open, the file will remain in existence until the last file descriptor referring to it is closed. If the name referred to a symbolic link, the link is removed. If the name referred to a socket, FIFO, or device, the name for it is removed but processes which have the object open may continue to use it. | `const char *pathname` | `0` on success or `-1` on error and errno is set appropriately |
| [dup](https://www.mkssoftware.com/docs/man3/dup.3.asp) |Duplica um File Descriptor. Quando da certo retorna um fd com o: Mesmo arquivo aberto (ou pipe) do arquivo original, mesmo ponteiro de arquivo (ambos os descritores de arquivo compartilham um ponteiro de arquivo), Mesmo modo de acesso (leitura, gravação ou leitura/gravação)|int dup(int fildes)|Success: Retorna um int com o fd duplicado, Error: Retorna -1 e define errno para o valor retornado|
| [dup2](https://www.mkssoftware.com/docs/man3/dup2.3.asp#:~:text=The%20dup2()%20function%20duplicates,any%20locks%20with%20the%20original.) |Duplica um File Descriptor aberto em outro File Descriptor|int dup(int fildes, int fildes2);|Success: Retorna um int com o fd duplicado, Error: Retorna -1 e define errno para o valor retornado|
| [opendir](https://www.delftstack.com/pt/howto/c/opendir-in-c/) |Acessa um fluxo de diretório correspondente ao diretório nomeado pelo argumento dirname . O fluxo de diretório é posicionado na primeira entrada. Se o tipo DIR for implementado usando um descritor de arquivo, os aplicativos só poderão abrir até um total de arquivos e diretórios {OPEN_MAX}.|DIR *opendir(const char *dirname);|Após a conclusão bem-sucedida, opendir () deve retornar um ponteiro para um objeto do tipo DIR . Caso contrário, um ponteiro nulo deve ser retornado e errno definido para indicar o erro.|
| [readdir](https://pubs.opengroup.org/onlinepubs/7908799/xsh/readdir.html#:~:text=The%20readdir()%20function%20returns,end%20of%20the%20directory%20stream.) |Lê um diretório, retorna um ponteiro para uma estrutura que representa a entrada de diretório na posição atual no fluxo de diretório especificado pelo argumento dirp e posiciona o fluxo de diretório na próxima entrada. Ele retorna um ponteiro nulo ao atingir o final do fluxo de diretório.|int readdir_r(DIR *dirp, struct dirent *entry, struct dirent **result**);|Success: readdir() retorna um ponteiro para um objeto do tipo struct dirent. Error: Quando um erro é encontrado, um ponteiro nulo é retornado e errno é definido para indicar o erro. Quando o final do diretório é encontrado, um ponteiro nulo é retornado e errno não é alterado.|
| [strerror](https://man7.org/linux/man-pages/man3/strerror.3.html)|Retorna um ponteiro para uma string que contém uma mensagem de erro para um errnum específico.|char *strerror(int errnum)|Success: A string de erro é retornada com codigo 0, Error: é retornada -1 e atribuido Errno ao número|
| [perror](https://man7.org/linux/man-pages/man3/perror.3.html) |Imprime uma mensagem de erro descritiva para stderr. Primeiro a string str é impressa, seguida por dois pontos e depois um espaço.|void perror(const char *str)|(none)|
| [isatty](https://man7.org/linux/man-pages/man3/isatty.3.html) |Testa se um File Descriptor se refere a um terminal.|int isatty(int fd);|Success: retorna 1 se o fd for um descritor de arquivo aberto referente a um terminal; Error: caso contrário, 0 é retornado e errno é definido para indicar o erro.|
| [ttyname](https://man7.org/linux/man-pages/man3/ttyname.3.html) |Retorna um ponteiro para uma string contendo um nome de caminho terminado em nulo do terminal associado aos fildes do descritor de arquivo. O valor de retorno pode apontar para dados estáticos cujo conteúdo é substituído por cada chamada.|char *ttyname(int fildes);| Success: retorna um ponteiro para um nome de caminho. Error: NULL é retornado e errno é definido como indicar o erro. Retorna 0 em sucesso e um número de erro em caso de erro.|
| [ttyslot](https://man7.org/linux/man-pages/man3/ttyslot.3.html) |Encontra o slot do terminal do usuário atual em alguns Arquivo.|int ttyslot(void);|Success: Retornará o número do slot. Em erro (por exemplo, se nenhum dos descritores de arquivo 0, 1 ou 2 estiver associado com um terminal que ocorre nesta base de dados) ele retorna 0 em Sistemas UNIX V6 e V7 e semelhantes a BSD, mas -1 em sistemas semelhantes a System V sistemas.|
| [ioctl](https://linuxhint.com/c-ioctl-function-usage/#:~:text=The%20Purpose%20of%20the%20IOCTL%20Function%20in%20C%3A&text=The%20device%20files%20are%20the,IOCTL%E2%80%9D%20function%20comes%20into%20play.) |Acessa arquivos especiais de sistema.|int ioctl(int fd, unsigned long request, ...);|Success: zero é retornado. Algumas solicitações ioctl() usam o valor de retorno como um parâmetro de saída e retornar um valor não negativo valor no sucesso. Error: -1 é retornado e errno é definido como indicar o erro.|
| [getenv](https://linuxhint.com/getenv-function-usage/) |Procura a string de ambiente apontada pelo nome e retorna o valor associado à string.|char *getenv(const char *name)|Success: Valor da variável de ambiente que foi buscada. Error: um ponteiro nulo se essa variável de ambiente não existir.|
| [tcsetattr](https://pubs.opengroup.org/onlinepubs/009696799/functions/tcsetattr.html) |Define os parâmetros associados ao terminal.|int tcsetattr(int fildes, int optional_actions, const struct termios *termios_p);|Success: 0 deve ser retornado. Error: -1 deve ser retornado e errno definido para indicar o erro.|
| [tcgetattr](https://pubs.opengroup.org/onlinepubs/007904975/functions/tcgetattr.html) |Deve obter os parâmetros associados ao terminal referido por fildes e armazená-los na estrutura termios referenciada por termios_p . O argumento fildes é um descritor de arquivo aberto associado a um terminal.|int tcgetattr(int fildes, struct termios *termios_p);|Success: 0 deve ser retornado. Caso contrário. Error: -1 deve ser retornado e errno definido para indicar o erro.|
| [tgetent](https://www.gnu.org/software/termutils/manual/termcap-1.3/html_chapter/termcap_2.html)|Consulta a descrição do tipo de terminal em uso|int tgetent (char *buffer, char *termtype);|Success: as funções que retornam um número inteiro retornam OK. Error: eles retornam ERR. Funções que retornam ponteiros retornam um ponteiro nulo em caso de erro.|
| [tgetflag](https://www.badprog.com/unix-gnu-linux-system-calls-using-tgetflag) |interroga o programa para vários recursos do terminal. Você deve especificar o código de duas letras do recurso cujo valor você procura chamado de capacidade. É um booleano que obtem um valor|int tgetflag (char *name);|Success: Se o nome do recurso estiver presente na descrição do terminal, tgetflag retornará 1; Error: retorna 0.|
| [tgetnum](https://linux.die.net/man/3/tgetnum) |Obter um valor de capacidade de tipo numérico|int tgetnum(char *id);|Success: retornará o valor numérico (que não é negativo). Se a capacidade não for mencionada na descrição do terminal, retornará -1.|
| [tgetstr](https://linux.die.net/man/3/tgetstr) |Obter um valor de capacidade de tipo String|char *tgetstr (char *name, char **area**);|Success: Ele retorna um ponteiro para uma string que é o valor da capacidade ou um ponteiro nulo se a capacidade não estiver presente na descrição do terminal.|
| [tgoto](https://linux.die.net/man/3/tgoto) |Trata o caso de especial de cursor|char *tgoto (char *cstring, int hpos, int vpos)|Success: retorna 1, 0 se não houver tal entrada e -1 se o banco de dados terminfo não puder ser encontrado.|
| [tputs](https://linux.die.net/man/3/tputs) |recupera recursos pelo nome termcap ou terminfo.|int tputs(const char *str, int affcnt, int (*putc)(int));|Success: retorna 1, 0 se não houver tal entrada e -1 se o banco de dados terminfo não puder ser encontrado.|
