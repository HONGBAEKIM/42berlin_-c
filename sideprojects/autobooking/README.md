# I want to make auto macro to book evaluation.
# because I do not want to spend time to click F5 several times.

# I am going to ask Chatgpt how to make struct or how to start to make a code.!

# chatgpt recommend free libraries for linux with python to make macro that can book evaluation slot automatically.

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

