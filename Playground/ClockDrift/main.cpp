#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


std::vector<double> get_values(const std::string& file)
{
    std::vector<double> result;

    std::ifstream ifs(file);
    double value = 0;
    while (ifs >> value)
    {
        result.push_back(value);
    }

    return result;
}


inline static double square(double x)
{
    return x*x;
}


int linear_regression(int n, const double x[], const double y[], double* m, double* b, double* r)
{
    double sumx = 0.0;                        /* sum of x                      */
    double sumx2 = 0.0;                       /* sum of x**2                   */
    double sumxy = 0.0;                       /* sum of x * y                  */
    double sumy = 0.0;                        /* sum of y                      */
    double sumy2 = 0.0;                       /* sum of y**2                   */

   int i;
   for ( i=0;i<n;i++)
      {
      sumx  += x[i];
      sumx2 += square(x[i]);
      sumxy += x[i] * y[i];
      sumy  += y[i];
      sumy2 += square(y[i]);
      }

   double denom = (n * sumx2 - square(sumx));
   if (denom == 0) {
       // singular matrix. can't solve the problem.
       *m = 0;
       *b = 0;
       *r = 0;
       return 1;
   }

   *m = (n * sumxy  -  sumx * sumy) / denom;
   *b = (sumy * sumx2  -  sumx * sumxy) / denom;
   if (r!=NULL) {
      *r = (sumxy - sumx * sumy / n) /          /* compute correlation coeff     */
            sqrt((sumx2 - square(sumx)/n) *
            (sumy2 - square(sumy)/n));
   }

   return 0;
}


int main()
{
    std::vector<double> raw_timevals = get_values("raw_timeval");
    std::vector<double> current_times = get_values("current_time");

    std::cout << "current_times.size=" << current_times.size() << std::endl;
    std::cout << "raw_timevals.size=" << raw_timevals.size() << std::endl;

    if (raw_timevals.size() != current_times.size())
    {
        std::cerr << "raw_timeval and current_times sizes differ" << std::endl;
        return 1;
    }

    std::vector<double> diffs;
    for (auto i = 0u; i != raw_timevals.size(); ++i)
    {
        auto diff = raw_timevals[i] - current_times[i];
        diffs.push_back(diff);
    }


    double m = 0;
    double b = 0;
    double r = 0;

    linear_regression(raw_timevals.size(), current_times.data(), raw_timevals.data(), &m, &b, &r);

    printf("m=%g b=%g r=%g\n", m, b, r);

}


