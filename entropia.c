#include <stdio.h>
#include <math.h>
#include <stdlib.h>

const int BUFFER_SIZE = 256;

void print_usage()
{
	printf("USAGE: entropia <sourcefile> [<min_numbit> [<max_numbit> [-v]]].\n");
	printf("  Calcola l'entropia e il massimo rapporto di compressione del file sorgente.\n");
	printf("  L'esperimento viene eseguito più volte con simboli a lunghezza fissa di N bit.\n");
	printf("  Se impostati i parametri, N va da <min_numbit> a <max_numbit>\n");
	printf("  Se <max_numbit> non è impostato assume lo stesso valore di <min_numbit> e l'esperimento viene eseguito una volta sola.\n");
	printf("  Se <numbit> non è impostato vale 1.\n");
	printf("  L'opzione -v esegue il programma in modalità verbose (selezionabile solo come ultimo parametro)\n");
	printf("VINCOLI: \n");
	printf("  <min_numbit> deve essere un numero intero positivo compreso tra 1 e 18.\n");
	printf("  <max_numbit> deve essere un numero intero positivo compreso tra <min_numbit> e 18.\n");
}

int main(int argc, char const *argv[])
{
	system("clear");

	//controllo parametri
	int numbit;
	int min_numbit;
	int max_numbit;
	int verbose = 0;
	if (argc < 2 || argc > 5)
	{
		print_usage();
		return 1;
	}
	if(argc == 2)
	{
		min_numbit = 1;
		max_numbit = 1;
	}
	if(argc == 3)
	{
		min_numbit = atoi(argv[2]);
		max_numbit = min_numbit;
		if(min_numbit < 1 || min_numbit > 18)
		{
			printf("ERRORE: <min_numbit> deve essere un intero compreso tra 1 e 18.\n\n");
			print_usage();
			return 1;
		}
	}
	if(argc == 4)
	{
		min_numbit = atoi(argv[2]);
		max_numbit = atoi(argv[3]);
		if(min_numbit < 1 || min_numbit > 18)
		{
			printf("ERRORE: <min_numbit> deve essere un intero compreso tra 1 e 18.\n\n");
			print_usage();
			return 1;
		}
		if(max_numbit < min_numbit || max_numbit > 18)
		{
			printf("ERRORE: <max_numbit> deve essere un intero compreso tra <min_numbit> e 18.\n\n");
			print_usage();
			return 1;
		}
	}
	if(argc == 5)
	{
		min_numbit = atoi(argv[2]);
		max_numbit = atoi(argv[3]);
		if(min_numbit < 1 || min_numbit > 18)
		{
			printf("ERRORE: <min_numbit> deve essere un intero compreso tra 1 e 18.\n\n");
			print_usage();
			return 1;
		}
		if(max_numbit < min_numbit || max_numbit > 18)
		{
			printf("ERRORE: <max_numbit> deve essere un intero compreso tra <min_numbit> e 18.\n\n");
			print_usage();
			return 1;
		}
		if(argv[4][0] == '-' && argv[4][1] == 'v' && argv[4][2] == '\0')
		{
			verbose = 1;
		}
		else
		{
			printf("ERRORE: parametro \"%s\" non riconosciuto\n", argv[4]);
			print_usage();
			return 1;
		}
	}

	double max_crs[max_numbit - min_numbit+1];
	double entropies[max_numbit - min_numbit+1];
	double max_max_crs;

	//apertura file sorgente
	char * filename = (char *)argv[1];
	FILE *fp = fopen(filename, "r");
	if (!fp)
	{
		printf("ERRORE: Impossibile aprire il file \"%s\"\n\n", filename);
		print_usage();
		return 1;
	}

	printf("ANALISI DEL FILE: %s\n", filename);

	//Ogni ciclo esegue l'analisi per simboli di una data lunghezza di bit.
	for (numbit = min_numbit; numbit <= max_numbit; ++numbit)
	{
		//inizializzazione variabili
		unsigned char buffer[BUFFER_SIZE];	//il buffer di lettura (in byte).
		int numsym = pow(2,numbit);			//il numero totale di simboli possibili.
		int symbols_occurrence[numsym];		//l'array di occorrenze dei simboli.
		int symbols[numsym];				//l'array di simboli
		double probs[numsym];				//l'array di probabilità dei simboli.
		int cursym[numbit];					//una variabile temporanea che contiene i bit dell'ultimo simbolo letto.
		int peek = 0;						//l'ultimo simbolo letto
		int i;
		int j;
		int k;
		int cursym_counter = 0;				//conta quanti bit sono stati estratti del simbolo corrente
		int symbol_counter = 0;				//conta quanti simboli sono stati letti

		//inizializzazione delle occorrenze
		for (i = 0; i < numsym; ++i)
		{
			symbols_occurrence[i] = 0;
			symbols[i] = i;
		}

		//ciclo di lettura del file
		while(!feof(fp)){

			//pulisce il buffer, inizializzandolo a -1.
			for (i = 0; i < BUFFER_SIZE; ++i)
			{
				buffer[i] = 0;
			}
			
			//legge un blocco dal file
			int n_read = fread(buffer, 1, BUFFER_SIZE, fp);
			for (i = 0; i < n_read && buffer[i]; ++i)
			{
				//estrae un bit alla volta
				for (j = 0; j < 8; ++j)
				{
					k = 8-j-1;
					cursym[cursym_counter++] = ((buffer[i]) & (1 << k)) >> k;

					//raggiunto il numero di bit necessari, incrementa di 1 l'occorrenza del simbolo estratto ed aggiorna i contatori
					if (cursym_counter >= numbit)
					{
						for (k = 0; k < numbit; ++k)
						{
							peek = (peek << 1) | cursym[k];
						}					
						symbols_occurrence[peek]++;
						symbol_counter++;
						peek = 0;
						cursym_counter = 0;
					}
				}
			}
		}

		//estrae l'ultimo simbolo che può essere rimasto incompleto
		if(cursym_counter > 0)
		{
			for (k = 0; k < numbit; ++k)
			{
				peek = (peek << 1) | cursym[k];
			}
			symbols_occurrence[peek]++;
			symbol_counter++;
		}

		//ordina gli array dei simboli per occorrenza più alta
		for (i = 0; i < numsym - 1; ++i)
		{
			int tmpso;
			int tmps;
			for (j = i+1; j > 0 && symbols_occurrence[j] > symbols_occurrence[j-1]; --j)
			{
				tmpso = symbols_occurrence[j-1];
				tmps = symbols[j-1];
				symbols_occurrence[j-1] = symbols_occurrence[j];
				symbols[j-1] = symbols[j];
				symbols_occurrence[j] = tmpso;
				symbols[j] = tmps;
			}
		}
		
		//calcola e stampa un grafico delle probabilità e il valore di entropia
		double entropy = 0;
		if(min_numbit != max_numbit && verbose)
			printf("--------------------------------------------\n");
		printf("Simboli a lunghezza fissa di %d bit.", numbit);
		if(verbose)
			printf("\n\nGrafico delle probabilità dei simboli (in ordine di occorrenze)\n");
		if(verbose)
		{
			if(numbit <= 7)
				printf("simbolo\toccorr\tprobab\t\tistogramma\n");
			else
				if (numbit <= 15)
					printf("simbolo\t\toccorr\tprobab\t\tistogramma\n");
				else
					printf("simbolo\t\t\toccorr\tprobab\t\tistogramma\n");
		}
		double max_prob;
		for (i = 0; i < numsym; ++i)
		{
			probs[i] = (double)symbols_occurrence[i] / (double)symbol_counter;
			if(probs[i] == 0)
			{
				break;
			}
			if(i==0) max_prob = probs[i];
			for (j = 0; j < numbit; ++j)
			{
				k = numbit-j-1;
				int bin_symbol = (symbols[i] & (1 << k)) >> k;
				if(verbose)
					printf("%d", bin_symbol);
			}
			entropy -= probs[i] * log2(probs[i]);
			if(verbose)
				printf("\t%d\t%f\t", symbols_occurrence[i], probs[i]);
			int prob = (int) round(probs[i]/max_prob * 100);
			for (j = 0; j < prob; ++j)
			{
				if(verbose)
					printf("#");
			}
			if(verbose)
				printf("\n");
		}
		if(verbose)
			printf("Gli istogrammi sono stati scalati in base al simbolo con maggior probabilità\n");
		printf("\nEntropia della sorgente: %f\n", entropy);
		entropies[numbit-min_numbit] = entropy;
		max_crs[numbit-min_numbit] = numbit/entropy;
		if(max_crs[numbit-min_numbit] > max_max_crs)
			max_max_crs = max_crs[numbit-min_numbit];
		printf("Max CR: %f\n\n", max_crs[numbit-min_numbit]);
		rewind(fp);
	}

	//chiusura file sorgente
	fclose(fp);

	//Grafico delle entropie e dei massimi CR al variare del numero di bit dei simboli.
	//Viene stampato solo se l'esperimento è stato eseguito più volte.
	if(min_numbit != max_numbit)
	{
		printf("--------------------------------------------\nGrafico delle entropie e dei massimi CR al variare del numero di bit dei simboli\n");
		printf("n.Bit\tEntropia\tMax CR\t\tIstogramma Max CR (scalato da 1 al massimo valore registrato fra tutti gli esperimenti)\n");
		for (numbit = min_numbit; numbit <= max_numbit; ++numbit)
		{
			printf("%d\t%f\t%f\t", numbit, entropies[numbit-min_numbit], max_crs[numbit-min_numbit]);
			int max_cr = (int) round((max_crs[numbit-min_numbit]-1)/(max_max_crs - 1)* 100);
			int j;
			for (j = 0; j < max_cr; ++j)
			{
				printf("#");
			}
			printf("\n");
		}
		printf("Gli istogrammi sono stati scalati ed assumono valori da 1 al caso con massimo valore di Max CR\n\n");
	}

	return 0;
}