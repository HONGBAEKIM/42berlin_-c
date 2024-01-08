# include "BitcoinExchange.hpp"

int main(int ac, char *av[]) 
{
    if (ac != 2)
    {
        std::cout << "argc != 2" << std::endl;
        return (1);
    }

    std::string inputfile(av[1]);
    
    BitcoinExchange bce = BitcoinExchange();
    try
    {
        bce.loadCsv("data.csv");
        bce.loadInput(inputfile);
    }
    catch(std::exception & e)
    {
        std::cout << e.what() << std::endl;
        return (1);
    }
    
    return (0);
}
