# Redes sem fio

Medir:
- Taxa de entrega;
- Atraso até o nó raiz;

A simulação desenvolvida no ns-3 gera um arquivo de saída contendo o pdr e o atraso de uma rodada de simulação. Foram gerados 200 arquivos para cada um dos 6 tipos de árvore. Esses 1200 arquivos são lidos por um script em python que gera um único arquivo, contendo a média e o intervalo de confiança para cada árvore.

Por fim, esse arquivo é lido pelo gnuplot (grafico.plot) a fim de gerar dois gráficos, o de pdr e o de atraso (ambos em formatos .eps e .png).

## Inclusão dos arquivos no ns-3
A tabela a seguir apresenta a localização dos arquivos neste repositório e o local para onde devem ser copiados.

| Arq. neste repo | Local de destino                           | Descrição                                                                                                         |
|-----------------|--------------------------------------------|-------------------------------------------------------------------------------------------------------------------|
| redes-adhoc.cc  | pastaDeInstalacaoDoNs3/scratch/          | arquivo principal da simulação                                                                                    |
| run.sh          | pastaDeInstalacaoDoNs3/                  | script responsável por executar todas as rodadas de simulações, para todos os cenários                            |
| processa.py     | pastaDeInstalacaoDoNs3/                  | calcula a média e intervalo de confiança com base nos arquivos de saída das simulações                            |
| grafico.plot    | pastaDeInstalacaoDoNs3/                  | gera gráficos nos formatos .eps e .png utilizando o gnuplot com base no arquivo de médias gerado pelo processa.py |
| applications/*  | pastaDeInstalacaoDoNs3/src/applications/ | aplicações Sender e Receiver                                                                                      |
| internet/model  | pastaDeInstalacaoDoNs3/internet/model/   | cabeçalho desenvolvido para armazenar o momento de envio de um pacote                                             |

## Execução
```
./run.sh
python processa.py
gnuplot grafico.plot
```

O script .sh demora para executar. Modifique-o caso queira distribuir o processamento.

## Links
- [stackoverflow - Compute a confidence interval from sample data](https://stackoverflow.com/questions/15033511/compute-a-confidence-interval-from-sample-data)
- [ns3::WifiMacQueue Class Reference](https://www.nsnam.org/doxygen/classns3_1_1_wifi_mac_queue.html#details)
- [HOWTO create a new type of protocol header or trailer](https://www.nsnam.org/wiki/HOWTO_create_a_new_type_of_protocol_header_or_trailer)
- [HOWTO make and use a new application](https://www.nsnam.org/wiki/HOWTO_make_and_use_a_new_application)
- [OnOffApplication](https://www.nsnam.org/doxygen/group__onoff.html)
