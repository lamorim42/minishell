
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
| rl_clear_history ||||
| rl_on_new_line | |||
| rl_replace_line | |||
| rl_redisplay | |||
| add_history |     |||
| waitpid |         |||
| wait3 |           |||
| wait4 |           |||
| signal |          |||
| sigaction |       |||
| sigemptyset |     |||
| sigaddset |       |||
| kill |            |||
| getcwd |          |||
| chdir |           |||
| stat |            |||
| lstat |           |||
| fstat |           |||
| unlink |          |||
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
