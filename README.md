
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
| dup |             |||
| dup2 |            |||
| opendir |         |||
| readdir |         |||
| strerror |        |||
| perror |          |||
| isatty |          |||
| ttyname |         |||
| ttyslot |         |||
| ioctl, |          |||
| getenv |          |||
| tcsetattr |       |||
| tcgetattr |       |||
| tgetent |         |||
| tgetflag |        |||
| tgetnum |         |||
| tgetstr |         |||
| tgoto |           |||
| tputs |           |||