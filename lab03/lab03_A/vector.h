#ifndef _VECTOR_H_
#define _VECTOR_H_ 

#include <iostream>

template <typename t, size_t size>
size_t len(t (&array)[size])
{
    return size;
}

namespace Data_Types{
    class Vector{
        private:
            static const int SZ = 10;
            int qsz;
            int ar[SZ];
            void swap(int &, int &);
        public:
            Vector(int size = 0, int value = 0);

            template <size_t size>
            Vector(const int (&elements)[size])
            {
                if(size > SZ)
                    throw std::runtime_error("Invalid size");

                qsz = size;
                int i;

                for(i = 0; i < size; i++)
                    ar[i] = elements[i];
                for(i; i < SZ ; i++)
                    ar[i] = 0;
            }

            Vector &input();
            Vector &add(const Vector &);
            // int getSize() const{ return qsz; }
            /*
            template <size_t size>
            int (& copy(int (&vector)[size], int s_index, int len))[size]
            {       
                if(s_index + len > qsz)
                    throw std::runtime_error("Invalid lenght");
                
                int e_index = s_index + len;
                int j = 0;

                for(int i = s_index; i < e_index; i ++){
                    vector[j] = ar[i];
                    j++;
                }

                return vector;
            }
            */
            Vector &copy(Vector &, int, int) const;
            Vector &push(int );
            Vector &sort();
            int max() const;
            std::ostream &print(std::ostream &) const;
    };
}


#endif