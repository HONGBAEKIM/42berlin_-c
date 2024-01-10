# How to use Auto macro booking evaluation slots?

# git clone 
# and follow 6 steps

# 0. download selenium
pip install selenium

# 1. you need to make .txt file to save your intra42 password
#########################################################
#########################################################
#########################################################
with open("../../../password.txt", "r") as password_file:
    password = password_file.read().strip()
#########################################################
#########################################################
#########################################################

# 2. Replace with the actual project name
#########################################################
#########################################################
#########################################################
project_name = "cpp-module-09"  
#########################################################
#########################################################
#########################################################

# 3. Go to your evaluation page and copy last team_id which is 5374260
example https://projects.intra.42.fr/projects/cpp-module-09/slots?team_id=5374260
#########################################################
#########################################################
#########################################################
#everyone has different team_id

#hongbae has "5374260"
#Ramesh has  "5395823"

team_id = "5374260"  # Replace with the actual team ID 
#########################################################
#########################################################
#########################################################

# 4. Set the desired time for the slot
#########################################################
#########################################################
#########################################################
desired_start_time = datetime.strptime("13:00", "%H:%M").time()  # 24-hour format
desired_end_time = datetime.strptime("23:45", "%H:%M").time()  # 24-hour format
#########################################################
#########################################################
#########################################################

# 5. if you believe my code comment out "nextok.click()"
###########last
###########last
###########last
#nextok.click()
###########last
###########last
###########last
I have not tried yet.

# 6. Run the program
in terminal type below 

python3 eval_booker8.py


I want to make auto macro to book evaluation.
because I do not want to spend time to click F5 several times.
I am going to ask Chatgpt how to make struct or how to start to make a code.!
chatgpt recommend free libraries for linux with python to make macro that can book evaluation slot automatically.


# Step by step

1. Selenium WebDriver

    Purpose: Automate web browser interaction from Python.
    Use Case: Can simulate a user navigating a web page, clicking on elements, filling out forms, and booking slots.
    Installation: Install using pip with the command pip install selenium.
    Additional: You'll need a driver for the browser you wish to automate (e.g., geckodriver for Firefox, chromedriver for Chrome).

2. Requests

    Purpose: Make HTTP requests to web servers.
    Use Case: Send GET and POST requests to log in to a website and manage sessions if direct web interaction is not needed.
    Installation: Install using pip with the command pip install requests.

3. BeautifulSoup

    Purpose: Parse HTML and XML documents.
    Use Case: Scrape data from web pages to find available slots.
    Installation: Install using pip with the command pip install beautifulsoup4.

4. lxml

    Purpose: Process XML and HTML in Python.
    Use Case: Use it as an alternative or in conjunction with BeautifulSoup for parsing and searching the document tree.
    Installation: Install using pip with the command pip install lxml.

5. Schedule

    Purpose: Run Python functions (or any other callable) periodically using a friendly syntax.
    Use Case: Schedule your script to run at specific times to check for open slots.
    Installation: Install using pip with the command pip install schedule.




