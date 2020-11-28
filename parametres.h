#ifndef PARAMETRES_H
#define PARAMETRES_H

class parametres{
    public:
        /*сгенерированная последовательность ПСЧ и её длина*/
        static long* seq;
        static long seqLen;

        /*параметры для ЛКГ*/
        static long a;
        static long b;
        static long c0;
        static const long n=24;
};

#endif // PARAMETRES_H
