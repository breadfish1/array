#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <assert.h>

using namespace std;

class matrix;
class row;

class row {
public:
    friend class matrix;
    int &operator [](int j);
private:
    int len, *str;
    row(int i);
};

class matrix {
public:
    row &operator [](int i);                              // перегрузка
    matrix &operator =(const matrix &a);                  // присваивание
    matrix operator +(matrix &a);                         // сложение
    matrix operator *(matrix &a);                         // умножение
    matrix &operator +=(matrix &a);                       // плюс-присвоить
    matrix &operator *=(matrix &a);                       // умножить-присвоить
    matrix &operator !();                                 // транспонировать
    friend ostream &operator <<(ostream &out, const matrix &m);
    matrix(int i, int j);                                 // конструктор по умолчанию
    matrix(const matrix &a);                              // копирующий конструктор
    row **getrow();                                       // получить строку
    ~matrix();                                            // деструктор
private:
    void del();                                           // очистить память
    void transp(row **x, int sz1, int sz2);               // транспонировать квадратную матрицу
    int rowsize() const;                                  // получить длинну строки матрицы
    int getstr(int i, int j) const;                       // получить [i][j] элемент
    row **create(int r, int c);                           // выделить память для двумерного [i][j] массива
    row **copy(row **a, row **b, int i, int j);           // скопировать b в a (только если одинаковые размерности)
    row **copytrspn(row **a, int i, int j);               // скопировать с транспонированием
    
    row **m;
    int len;
};

row::row(int i) {
    assert(i > 0);
    str = new int[i];
    len = i;
}

int &row::operator [](int j) {
    return str[j];
}

row &matrix::operator [](int i) {
    return *m[i];
}

matrix &matrix::operator =(const matrix &a) { // присваивание
    if (this == &a)
        return *this;
    
    if (this->len != a.len && this->rowsize() != a.rowsize()) {
        row **temp = create(a.len, a.rowsize());
        len = a.len;
        del();
        m = temp;
    }
    copy(m, a.m, a.len, a.rowsize());
    
    return *this;
}

matrix matrix::operator +(matrix &a) { // сложение
    int c = this->rowsize(), b = a.rowsize();
    assert(len == a.len && c == b);
    
    matrix n(*this);
    n += a;
    
    return n;
}

matrix &matrix::operator *=(matrix &a) { // умножить-присвоить
    assert(rowsize() == a.len);
    
    row **x = create(len, a.rowsize());
    
    int temp;
    
    for (int i = 0; i < len; i++)
        for (int j = 0; j < a.rowsize(); j++) {
            temp = 0;
            for (int f = 0; f < a.len; f++)
                temp += m[i]->str[f] * a.m[f]->str[j];
            x[i]->str[j] = temp;
        }
    
    del();
    m = x;
    
    return (*this);
}

matrix matrix::operator *(matrix &a) { // умножение
    assert(rowsize() == a.len);
    
    matrix proizv(*this);
    proizv *= a;
    
    return proizv;
}

matrix &matrix::operator +=(matrix &a) { // сложить-присвоить
    int c = this->rowsize(), b = a.rowsize();
    
    assert(len == a.len && c == b);
    
    for (int i = 0; i < len; i++)
        for (int j = 0; j < c; j++)
            m[i]->str[j] += a.m[i]->str[j];
    
    return *this;
}

ostream &operator << (ostream &out, const matrix &m) { // вывод
    if (&m == NULL) {
        out << "empty" << endl;
        return out;
    }
    
    int c = m.len, r = m.rowsize();
    
    for (int i = 0; i < c; i++) {
        for (int j = 0; j < r; j++) {
            out << m.getstr(i, j) << " ";
        }
        out << endl;
    }
    
    out << endl;
    
    return out;
}

void matrix::transp(row **x, int sz1, int sz2) { // транспонирование квадратной матрицы
    int i, j, temp;
    for (i = 0; i < sz1; i++)
        for(j = i + 1; j < sz2; j++)
        {
            temp = x[i]->str[j];
            x[i]->str[j] = x[j]->str[i];
            x[j]->str[i] = temp;
        }
}

matrix &matrix::operator !(){ // транспонирование
    if (len == rowsize()) // если матрица квадратная
        transp(m, len, rowsize());
    else {
        int a = len, b = rowsize();
        row **temp = copytrspn(m, a, b);
        
        del();
        m = temp;
        len = b;
    }
    
    return (*this);
}

int matrix::rowsize() const { // получить кол-во столбцов
    return m[0]->len;
}

int matrix::getstr(int i, int j) const {
    return m[i]->str[j];
}

matrix::matrix(int i, int j) {
    m = create(i, j);
    len = i;
}

matrix::matrix(const matrix &a) : len(a.len) {
    m = create(a.len, a.rowsize());
    m = copy(m, a.m, len, a.rowsize());
}

row **matrix::getrow() {
    return m;
}

matrix::~matrix() {
    del();
}

void matrix::del() { // очистить память
    for (int k = 0; k < len; k++) {
        delete[] m[k]->str;
        delete m[k];
    }
    
    delete[] m;
}

row **matrix::create(int r, int c) { // выделить память
    row **x = new row *[r];
    for (int k = 0; k < r; k++)
        x[k] = new row(c);
    
    return x;
}

row **matrix::copy(row **a, row **b, int rows, int columns) { // копировать b в a
    int i, j;
    
    for (i = 0; i < rows; i++)
        for (j = 0; j < columns; j++)
            a[i]->str[j] = b[i]->str[j];
    
    return a;
}

row **matrix::copytrspn(row **a, int rows, int columns) { // копирование с транспонированием
    row **temp = create(columns, rows);
    
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            temp[j]->str[i] = a[i]->str[j];
    
    return temp;
}

int main() {
    matrix x(5, 5), z(5, 5), y(3, 2);
    int a = 1;
    
    //x[2][3] = 4;
    
    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++) {
            x[i][j] = a++;
            z[i][j] = a;
        }
    
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 2; j++)
            y[i][j] = j + 5;
    
    cout << x << z << y;
    
    x += z;
    
    cout << x;
    
    !y;
    
    cout << y;
    
    //cout << x;
    
    //cout << z;
    
    x *= z;
    
    cout << x;
    
    
    
}
