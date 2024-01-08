/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PmergeMe.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hongbaki <hongbaki@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/05 14:02:25 by hongbaki          #+#    #+#             */
/*   Updated: 2024/01/04 18:44:46 by hongbaki         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PmergeMe.hpp"

/* 
What is Ford-Johnson algorithm?
The Ford-Johnson algorithm is specifically known for 
its efficiency in sorting small to medium-sized lists

1.Divide the Array (Merge Sort Part)
2.Recursively Sort Each Half
3.Merge the Sorted Halves (Merge Sort Part)

for example
[3 5 9 7 4]

Splitting list: <3, 5, 9, 7, 4>
Splitting list: <3, 5, 9> 
Splitting list: <3, 5>
Merging sublists: <3> <5>
Merged state: <3, 5>
Sorted sublist: <3, 5>
Merging sublists: <3, 5> <9>
Merged state: <3, 5, 9>
Sorted sublist: <3, 5, 9>
Splitting list: <7, 4>
Merging sublists: <7> <4>
Merged state: <7, 4>
Sorted sublist: <4, 7>
Merging sublists: <3, 5, 9> <4, 7>
Merged state: <3, 5, 9, 4, 7>
Sorted sublist: <3, 4, 5, 7, 9>
----------------------------------------------------------*/

PmergeMe::PmergeMe()
{
}

PmergeMe::~PmergeMe()
{
}

PmergeMe::PmergeMe(const PmergeMe &other)
{
    this->intVector = other.intVector;
    this->intDeque = other.intDeque;
}

PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
    if (this != &other)
    {
        this->intVector = other.intVector;
        this->intDeque = other.intDeque;
    }
    return (*this);
}

void PmergeMe::intVectorPrinter()
{
    for (std::vector<int>::iterator it = intVector.begin(); it != intVector.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
}

void PmergeMe::intDequePrinter()
{
    for (std::deque<int>::iterator it = intDeque.begin(); it != intDeque.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
}

void PmergeMe::readInput(char **argv)
{
    for (int idx = 1; argv[idx]; idx++)
    {
        intVector.push_back(std::atoi(argv[idx]));
        intDeque.push_back(std::atoi(argv[idx]));
    }
}

void PmergeMe::fordJohnsonSortWrapper()
{
    double duration;
    clock_t start, stop;

    std::cout << "Before: ";
    intVectorPrinter();
    
    start = clock();
    fordJohnsonSort(intVector, 0, intVector.size() - 1);
    stop = clock();
    
    std::cout << "After:  ";
    intVectorPrinter();
    duration = ((stop - start) / (double)CLOCKS_PER_SEC) * 1000000;
    std::cout << "Time to process a range of " << intVector.size() << " elements with std::vector: ";
    std::cout << duration << " us" << std::endl;

    start = clock();
    fordJohnsonSort(intDeque, 0, intDeque.size() - 1);
    stop = clock();
    
    duration = ((stop - start) / (double)CLOCKS_PER_SEC) * 1000000;
    std::cout << "Time to process a range of " << intDeque.size() << " elements with std::deque: ";
    std::cout << duration << " us" << std::endl;
}

template <typename T>
void PmergeMe::mergeSublists(T &lst, int lst_start_idx, int mid, int lst_end_idx)
{
    int left_idx = lst_start_idx;
    int right_idx = mid + 1;

    // Temporary vector to store merged sublists
    std::vector<int> temp(lst_end_idx - lst_start_idx + 1);
    int temp_index = 0;
    //Merge the Sublists
    while (left_idx <= mid && right_idx <= lst_end_idx)
    {
        if (lst[left_idx] <= lst[right_idx])
            temp[temp_index++] = lst[left_idx++];
        else
            temp[temp_index++] = lst[right_idx++];
    }
    //If any elements are left in the first sublist
    //they are added to temp.
    while (left_idx <= mid)
        temp[temp_index++] = lst[left_idx++];
    //if any elements are left in the second sublist
    //they are also added to temp.
    while (right_idx <= lst_end_idx)
        temp[temp_index++] = lst[right_idx++];

    //Combines the two sorted sublists into a single sorted segment in the original list.
    for (int i = 0; i < temp_index; i++)
        lst[lst_start_idx + i] = temp[i];
}

template <typename T>
void PmergeMe::fordJohnsonSort(T &lst, int lst_start_idx, int lst_end_idx)
{
    int lst_mid_idx;

    if (lst_start_idx != lst_end_idx)
    {
        lst_mid_idx = lst_start_idx + ((lst_end_idx - lst_start_idx) / 2);
        fordJohnsonSort(lst, lst_start_idx, lst_mid_idx);
        fordJohnsonSort(lst, lst_mid_idx + 1, lst_end_idx);
        mergeSublists(lst, lst_start_idx, lst_mid_idx, lst_end_idx);
    }
}

// Explicit instantiation of template for specific types
template void PmergeMe::fordJohnsonSort(std::vector<int> &lst, int lst_start_idx, int lst_end_idx);
template void PmergeMe::fordJohnsonSort(std::deque<int> &lst, int lst_start_idx, int lst_end_idx);
