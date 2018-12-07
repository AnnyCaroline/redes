# Redes sem fio

Medir:
- Taxa de entrega;
- Atraso até o nó raiz;

A simulação desenvolvida no ns-3 gera um arquivo de saída contendo o pdr e o atraso de uma rodada de simulação. São gerados 100 arquivos para cada um dos 6 tipos de árvore. Esses 600 arquivos são lidos por um script em python que gera um único arquivo, contendo a média e o intervalo de confiança para cada árvore.

Por fim, esse arquivo é lido pelo gnuplot (grafico.plot) a fim de gerar dois gráficos, o de pdr e o atraso (ambos em formatos .eps e .png).

## Execução
```
./run.sh
python processa.py
gnuplot grafico.plot
```

O script .sh pode demorar cerca de 3 horas para executar. Modifique-o caso queira distribuir o processamento.

## Links
- [stackoverflow - Compute a confidence interval from sample data](https://stackoverflow.com/questions/15033511/compute-a-confidence-interval-from-sample-data)
