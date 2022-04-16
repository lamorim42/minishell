
# Onde estamos

Estamos fazendo apenas a tokenização, após esse momento analizamos se no token
tem ou não o par de aspas. Se tiver vamos para função que explode as aspas, se
não tiver vamos para função que abre o here_doc.

- **exemplo:**

"ls -a -> abre here_doc -> recebe restante do comando até as aspas ->
substitui o token ("ls -a) por ("ls -a o_que_vem_do_here_doc") e provalvemente
vai dar ruim.

# Não esquecer que quando arrays de strings começarem com aspas que não são fechadasm, as words são contadas como uma!!