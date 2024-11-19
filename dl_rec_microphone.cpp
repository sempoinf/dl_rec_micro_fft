// #include <cstlib>
#include <iostream>
// #include <math.h>
#include <string>
#include <fstream> // working with file 

#include <vector>
#include <numeric> // for std::accumulate
#include <regex> // match to pattern

#include <fftw3.h>
#include <stdexcept> // ?

#define F_SAMPLING 5000

// using std::string;
using namespace std;

class ParserTxt
{
    private:    // Access specifier
        string filename;           // Name of the file to be processed
        // vector<int16_t> cur_results;       // Vector to store current session results
        vector<vector<int16_t>> results; // Stores arrays for each session

    public:
        // Constructor to initialize the filename
        explicit ParserTxt(const string& file) : filename(file) {}
        
        ~ParserTxt() = default;

        // Setter for filename
        void setFile(const string& file) 
        {
            filename = file;
        }
       
        // Getter for filename
        const string getFile() const 
        {
            return filename;
        }

        // Read and print all lines from the file
        void readFile() const 
        {
            ifstream myfile(filename);
            if (!myfile)
            {
                throw runtime_error("Unable to open file for reading");
            }

            string line;
            while (getline(myfile, line)) 
            {
                cout << line << endl;
            }
        }

        // Extract all results from the file that match the pattern
        void parseResults(const string& regex_pattern) 
        {
            regex pattern(regex_pattern);
            smatch match;

            ifstream myfile(filename);
            if (!myfile) 
            {
                throw runtime_error("Unable to open file for reading");
            }

            string line;
            vector<int16_t> cur_results;  
            // cur_results.clear(); // Clear previous results before parsing new ones
            // Counter finded lines
            // int16_t cnt{0};

            while (getline(myfile, line)) 
            {
                if (line == "END OF DATA")
                {
                    if (!cur_results.empty()) 
                    {
                        // for(auto& result : cur_results)
                        // {
                        //     cout << result << ' ';
                        // }
                        // cout << endl;
                        results.push_back(cur_results); // Store the session
                        cur_results.clear();            // Reset for the next session
                    }
                }
                else if (regex_search(line, match, pattern)) 
                {
                    // Extract the value and add it to the current session
                    cur_results.push_back(stoi(match[1].str()));
                }
            }
            // cout << cnt << endl;
        }
        
        // Return pointer to the first element of results vector, or nullptr if empty
        auto* getResultPtr(const int16_t index) 
        {
            return results.empty() ? nullptr : &results.at(index).at(0);
        }
        
        auto getSizeResult(const int16_t index)
        {
            return results.empty() ? 0 : results[index].size();
        }

        auto getSizeResults()
        {
            return results.empty() ? 0 : results.size();
        }

        int16_t* getResultPtrSize(int16_t& size, const int16_t index) 
        {
            if(!results.empty())
            {
                size = results.size();
                return &results.at(index).at(0);
            }
            else
            {
                return nullptr;
            }
            
        }

        int16_t findAvg(const int16_t index)
        {
            int32_t sum = accumulate(results[index].begin(), results[index].end(), 0);
            // std::cout << "Sum: " << sum << std::endl;
            double avg = static_cast<double>(sum) / results[index].size();
            // std::cout << "Avg: " << avg << std::endl;
            return static_cast<int16_t>(avg);
        }

        void center_Result(const int16_t index, int16_t avg_b)
        {
            for (auto& result : results[index])
            {
                result -= avg_b;
            }
        }

        void printResults() const 
        {
            if (results.empty()) 
            {
                cerr << "No results to display."<< endl;
                return; 
            }
            else
            {
                for (const auto& vec : results)
                {   
                    if (!vec.empty())
                    {
                        for (const auto& value : vec) 
                        {
                            cout << value << ' ';
                        }
                    }
                    else
                    {
                        cout << "vec is empty" << endl;
                    }
                    cout << endl;
                } 
            }
        }

        // Print all results stored in the vector
        void printResultsArIndex(int16_t index) const 
        {   
            if (results.empty()) 
            {
                cerr << "No results to display."<< endl;
                return; 
            }
            else if (index < results.size()) 
            {
                // Get a link on inner vector
                const vector<int16_t>& vec = results.at(index);
                for (const int16_t& value : vec) 
                {
                    cout << value << " ";
                }
                cout << endl;
            }
            else
            {
                cout << "Wrong Index." << endl;
                return;
            }
        }            
};


class fft3w
{
    private:
        int16_t size; // size of buffer / vector
        vector<double> input;        // input buffer / vector
        vector<double> output;       // output buffer / vector
        fftw_plan plan;                    // FFT plan to execute
    public:
        fft3w(int size) 
            : size(size), 
            input(size), 
            output(size) 
        {
            // Plan FFT for 1 dimensity transform
            // plan = fftw_plan_dft_1d(size, input.data(), output.data(), FFTW_FORWARD, FFTW_ESTIMATE);
            plan = fftw_plan_r2r_1d(size, input.data(), output.data(), FFTW_DHT, FFTW_ESTIMATE);
        }
        ~fft3w() 
        {
            // Освобождение ресурсов FFT
            fftw_destroy_plan(plan);
            fftw_cleanup();
            // fftw_free(input);
            // fftw_free(output);
        }
        void setInput(const int16_t *data) 
        {
            // Set input buffer
            for (int16_t i{0}; i < size; ++i) 
            {
                input[i] = data[i];
                // cout << input[i] << endl;
            }
        }

        void execute(void)
        {
            fftw_execute(plan);
        }

        double calculate_freq()
        {
            int16_t peak_i{0};
            double max_amplit{0.0};
            for (int16_t i{0}; i < size; ++i)
            {
                if (fabs(output[i]) > max_amplit) 
                {
                    max_amplit = fabs(output[i]);
                    peak_i = i;
                }
            }

            double signal_frequency = (peak_i * F_SAMPLING) / size;
            return signal_frequency;
        }

};

int main()
{
    // write something
    // ofstream myfile;
    // myfile.open("../dl_rec_read_data_py/res/results_500Hz.txt");
    // myfile.close();

    // read something
    // ifstream myfile("example.txt");
    // if (myfile.is_open())
    // {
    //     string line;
    //     // cout << '1';
    //     while(getline(myfile, line))
    //     {
    //         cout << line << std::endl;
    //     }
    //     myfile.close();
    // }
    // else 
    // {
    //     std::cerr << "Unable to open file for reading" << std::endl;
    // }

    ParserTxt txt("../dl_rec_read_data_py/res/results_500Hz.txt");
    // txt.setFile("example.txt");
    // cout << txt.getFile();
    
    // txt.readFile();
    txt.parseResults(R"(Just value tick \d+: (\d+) mV)");
    // txt.printResults();
    int16_t out_size = txt.getSizeResults();
    // cout << out_size << endl;
    
    // txt.printResultsArIndex(1);
    const int16_t session{1};
    int16_t* pRes = txt.getResultPtr(session);
    cout << *pRes << endl;
    // cout << *(pRes+1) << endl; // second elem in first array

    // int16_t* pRes1 = txt.getResultPtr(1);
    // cout << *pRes1 << endl;

    int16_t inner_size = txt.getSizeResult(session);
    cout << inner_size << endl;

    // int16_t size(0); 
    // int16_t* pRes = txt.getResultPtrSize(size, 0);
    // cout << size << endl;
    // cout << *pRes << endl;

    int16_t avg_buff = txt.findAvg(session);
    // cout << avg_buff << endl;

    txt.center_Result(session, avg_buff);
    // cout << *pRes << endl;
    txt.printResultsArIndex(session);

    fft3w fftProcessor(inner_size);
    fftProcessor.setInput(pRes);    // Set input data
    fftProcessor.execute();         // Start FFT

    double signal_freq = fftProcessor.calculate_freq();

    cout << "Frequency of signal: " << signal_freq << endl;

    // g++ -O2 -std=c++17 dl_rec_microphone.cpp -o dl_rec_microphone
    return 0; 
}