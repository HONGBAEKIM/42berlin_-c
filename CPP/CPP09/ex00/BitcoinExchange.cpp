# include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange()
{
    //Compare two ''struct tm' objects(which represent calendar time)
    bool(*function_pt)(struct tm const &, struct tm const &) = &timeCompare;

    //exchage variable is a map that stores pairs of struct tm and float values.
    //it uses the timeCompare function to sort and compare elements
    exchange = std::map<struct tm, float, bool(*)(struct tm const &, struct tm const &)>(function_pt);
}

BitcoinExchange::~BitcoinExchange()
{
    
}

BitcoinExchange::BitcoinExchange(BitcoinExchange const & btc)
{
    *this = btc;
}

BitcoinExchange &BitcoinExchange::operator=(BitcoinExchange const & rhs)
{
    if (this != &rhs)
        exchange = rhs.exchange;
    return *this;
}

void BitcoinExchange::loadCsv(const std::string& filename)
{
    iterFile(filename, &BitcoinExchange::processCsvEntry);
}

//iterFile reads a file line by line, 
//parses each line to extract data, 
//and applies a specific operation (member function of BitcoinExchange) to each piece of extracted data. 
void BitcoinExchange::iterFile(std::string const &filename, void (BitcoinExchange::*entry_ptr)(std::pair<struct tm, float>))
{
    //fstream is reading from and writing to files
    //std::ios::in: This flag indicates that the file is to be opened for input operations.
    std::fstream fs (filename.c_str(), std::ios::in);
    
    if (fs.fail())
        std::cerr << "Error: Unable to open file '" << filename << "'\n";
    //way to configure the stream to throw exceptions of type 
    fs.exceptions(fs.failbit);

    std::string delimiter = "|";
    //filename.rfind(".csv") returns 4,
    //filename is 'data.csv' cus the substring ".csv" start at index 4('d=0', 'a=1', 't=2', 'a=3', '.=4') 
    bool csv = (filename.rfind(".csv") + 4 == filename.length());
    if (csv)
        delimiter = ",";
    try
    {
        std::string line;
        //Read line from the file stream 'fs' and store it in 'line'
        std::getline(fs, line);
        //when a non-recoverable error occurs
        fs.exceptions(std::fstream::badbit);

        while (std::getline(fs, line))
        {
            try
            {
                //'entry' is variable
                //'extractPair' is to parse the 'line' based on the 'delimiter' into a pair of struct tm and float
                std::pair<struct tm, float> entry = extractPair(line, delimiter);
                //entry_ptr is the pointer to 'BitcoinExchange'.
                //entry is a std::pair<struct tm, float>.
                (this->*entry_ptr)(entry);
            }
            catch (BitcoinExchange::BadInputException & e)
            {
                if (csv)
				{
					fs.close();
					throw;
				}
				else
					std::cout << e.what() << std::endl;
            }
            catch (BitcoinExchange::BadDateException & e)
			{
				if (csv)
				{
					fs.close();
					throw;
				}
				else
					std::cout << e.what() << std::endl;
			}
            catch (BitcoinExchange::BadNumberException & e)
			{
				if (csv)
				{
					fs.close();
					throw;
				}
				else
					std::cout << e.what() << std::endl;
			}
			
        }
    }
    
    //std::ios_base::failure is an exception class in the C++ Standard Library 
    //that is used to report errors that occur during input/output operations.

    catch(std::ios_base::failure & e)
    {
        // Close the file stream before re-throwing the exception
        fs.close();
        // Re-throw the exception for further handling
        throw;
    }
    // Close the file stream if no exceptions were thrown
    fs.close();
}

void BitcoinExchange::processCsvEntry(std::pair<struct tm, float> entry)
{
    exchange.insert(entry);
}

std::pair<struct tm, float> BitcoinExchange::extractPair(std::string const & line, std::string const & delimiter) const
{
    size_t  pos = 0;
    std::string tmp = "";
    struct tm   timedata = {};
    float   value = 0;
    
    //ex)          '2017-03-14 | 1'
    //      pos is             ^ here
    pos = line.find(delimiter);

    //if the delimiter is not found or found at the beginning of the line
    if (pos == std::string::npos || pos <= 9)
        throw (BadInputException());
    
    //ex)           '2017-03-14 | 1'
    //     tmp is    2017-03-14 
    tmp = line.substr(0, pos);
    if (badDate(tmp))
        throw (BadDateException());
    //strptime is a function that parse the date string (tmp) into the data struct.
    //tmp.c_str() : converts the string to a const char*
    //%Y-%m-%d %H:%M:%S : format of the string
    //&data : the struct tm to store the parsed data
    strptime(tmp.c_str(), "%Y-%m-%d %H:%M:%S", &timedata);

    //Extract the value from the line, starting from the position 'pos + 1' to the end of the line
    tmp = line.substr(pos + 1);
    //convert the string 'tmp' to a float 'value'
    std::istringstream iss(tmp);
    //Read a 'float' value from the stream 'iss'
    
    //when extraction of float failed
    //ex)           '2017-03-14 | 1'
    //               2017-03-14 | (  ) <-- case is not a number after the delimiter
    size_t firstnonNumeric = tmp.find_first_not_of("0123456789.");
    if (firstnonNumeric != std::string::npos)
    {
        size_t secondNonNumeric = tmp.find_first_not_of("0123456789.", firstnonNumeric + 1);
        if (secondNonNumeric != std::string::npos)
            throw (BadNumberException());
    }
    //read a float from the input stream iss and store it in value
    if (!(iss >> value))
        throw (BadNumberException());

    //ex) '2017-03-14 | 1'
    //     ^^^^^^^^^^data address is stored in timedata
    
    //ex) '2017-03-14 | 1'
    //                  ^value is stored in value
    return std::pair<struct tm, float>(timedata, value);
}


void BitcoinExchange::loadInput(const std::string& filename)
{
    iterFile(filename, &BitcoinExchange::processInputEntry);
}

void BitcoinExchange::processInputEntry(std::pair<struct tm, float> entry)
{
    if (entry.second < 0.0f)
    {
        std::cout << "Error: Positive integer should be >= 0," << std::endl;
        return ;
    }
    if (entry.second > 1000.0f)
    {
        std::cout << "Error: Positive integer should be <= 1000," << std::endl;
        return ;
    }
    //entry is used within the iterFile and processInputEntry functions 

    //while exchange appears to be the main data structure 
    //where these processed data points are eventually stored and managed.
    
    //if(timecompare(input.txt , data.csv))
    if (timeCompare(entry.first, (*exchange.begin()).first))
    {
        std::cout << "Error : date is earlier" << std::endl;
        return ;
    }
    char date[11];
    //strftime is a function that converts the data struct into a string
    strftime(date, 11, "%Y-%m-%d", &entry.first);
    std::cout << date << " => " << entry.second << " = " << convert(entry) << std::endl;

}

float	BitcoinExchange::convert(std::pair<struct tm, float> entry) const
{
    std::map<struct tm, float, bool(*)(const struct tm&, const struct tm&)>::const_iterator it;
    //'exchange' map to find the most relevant exchange rate for a given date
    //upper_bound finds the most relevant data point
    it = exchange.upper_bound(entry.first);
    
    //if the given date is earlier than the first date in the data.csv
    it--;

    //(entry.first is the date of bitcoin (input.txt)
    //(entry.second is the amount of bitcoin (input.txt)

    //(*it).first is the date of bitcoin (data.csv)
    //(*it).second is the exchange rate of bitcoin (data.csv)
    return (entry.second * (*it).second);
}

bool timeCompare(const struct tm& a, const struct tm& b)
{
    if (a.tm_year == b.tm_year)
    {
        if (a.tm_mon == b.tm_mon)
           return (a.tm_mday < b.tm_mday);
        return (a.tm_mon < b.tm_mon);
    }
    return (a.tm_year < b.tm_year);
}

bool badDate(std::string tmp)
{
    std::stringstream ss;
    //0000-XX-00
    //month
    ss << tmp.substr(5, 2);
    unsigned int month;
    //'>>' is extract value
    ss >> month;
    ss.clear();
    //0000-00-XX
    //date
    ss << tmp.substr(8, 2);
    unsigned int day;
    ss >> day;

    // Check if the extracted month and day are within valid ranges
    if (month < 1 || month > 12 || day < 1 || day > 31)
        return true;

    return false;
}
