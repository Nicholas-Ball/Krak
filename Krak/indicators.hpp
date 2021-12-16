#include <vector>
#include <cmath>
#include <iostream>

inline double PercentChange(double initial, double final)
{
    return ((final - initial)/(fabs(initial)))*100.0;
}

//invert vector array
template <typename t>
std::vector<t> InvertArray(std::vector<t> arr)
{
    std::vector<double> out;
    for(int i = arr.size()-1;i != -1;i--)
    {
        out.push_back(arr[i]);
    }
    return out;
}

class Indicator
{
    public:
        //Calculate ema (index 0 = current day)
        static std::vector<double> EMA(std::vector<double> arr,int period)
        {
            std::vector<double> output;

            output.push_back(SMA(arr)[period]);

            //invert
            arr = InvertArray(arr);

            double multiplier = 2.0/(1.0+(double)period);

            for(int i = period; i !=arr.size();i++)
            {
                //ema = price * multipiler + prevoiusEma * (1-multiplier)
                output.push_back(arr[i]*multiplier+output[output.size()-1]*(1-multiplier));
            }
            return InvertArray(output);
        }
        //simple moving average (index 0 = current day)
        static std::vector<double> SMA(std::vector<double> arr)
        {
            std::vector <double> sma;
            sma.push_back((arr[0]+arr[1])/2.0);

            for(int i = 2; i != arr.size();i++)
            {
                sma.push_back((sma[i-1]+arr[i])/2);
            }
            return InvertArray(sma);
        }
        //macd calculations (index 0 = current day)
        static std::vector<double> MACD(std::vector<double> arr)
        {
            //invert array
            //arr = InvertArray(arr);

            std::vector<double> temp;

            std::vector<double> out;
            
            //loop through inputed
            for(int i = arr.size()-1; i != -1 ;i--)
            {
              if(i < arr.size()-26)
              {
									auto ema12 = EMA(temp,12);
									auto ema26 = EMA(temp,26);
                  out.push_back(ema12[ema12.size()-1] - ema26[ema26.size()-1]);
              }
              temp.push_back(arr[i]);
            }

          return InvertArray(out);
        }
        //rsi calculations (index 0 = current day)
        static double RSI(std::vector<double> arr,int period)
        {   
            double value;
            if(arr.size() >= period){

                //invert array
                arr = InvertArray(arr);

                double PreviousDay = arr[0];
                double up = 0;
                double down = 0;

                //calculate first perod's rsi
                for(int i = 0;i != period;i++)
                {
                    //calculate diference of today with n-1 day
                    double difference =  PercentChange(PreviousDay,arr[i]);

                    if(difference >= 0){
                        up = (up+difference)/2;
                        difference = 0;
                        down = (fabs(difference)+down)/2;
                    }
                    else
                    {
                        down = (fabs(difference)+down)/2;
                        difference = 0;
                        up = (up+difference)/2; 

                    }
                    PreviousDay = arr[i];
                }
                //calculate first step rsi
                value = 100-(100/(1+(up/down)));

                //calculate remaing days
                for(int i = period; i != arr.size();i++)
                {
                    //calculate diference of today with n-1 day
                    double difference =  PercentChange(PreviousDay,arr[i]);

                    double cup = 0;
                    double cdown = 0;

                    if(difference >= 0){
                        up = fabs((up+difference)/2);
                        cup = fabs(difference);
                        difference = 0;
                        down = (fabs(difference)+down)/2;
                    }
                    else
                    {
                        down = (fabs(difference)+down)/2;
                        cdown = fabs(difference);
                        difference = 0;
                        up = fabs((up+difference)/2);

                    }

                    PreviousDay = arr[i];

                    value = 100-(100/(1+(((up*13)+cup)/((down*13)+cdown))));
                }
            }
            return value;
        }
};