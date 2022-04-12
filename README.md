# TODO

- [ ] passar a trim no line->cmds_table

- [ ] criar map para passar a trim

- [x] array de cada comando

- [ ] testes

# Tests

- miau"|""dasdas" = word pipe word

- "miau"|miau" = erro

- l"s" -a "|" "miau" = word word pipe word

- adasd huasd ashdau| Z| haud | asduas

- "cat"|"c"at|c"a"t|ca"t"|" cat"|"cat -e "

- ca"t"|" cat"| = COUNT TA ERRADO

# Idias

- Usar Hashtable (ela guarda os valores da variavel PATH do sistema e é com ela

- que conseguimos construir os builtins) para verificar as builtins

## Valgrind flags

valgrind --leak-check=full --show-leak-kinds=all --suppressions=readline.supp ./minishell

## Init Execve
- Testar no começo com um comando apenas ate o execve funcionar
- testar com pipes
- testar com varios pipes

## Flags
flag para verificar quais file descriptors estão abertos no fim da execução do programa
	valgrind --track-fds=yes
flag para verificar leaks de memória nos processos filhos
	valgrind --trace-children=yes


# fazer testes para lidar com mais pipes no exec_pipe