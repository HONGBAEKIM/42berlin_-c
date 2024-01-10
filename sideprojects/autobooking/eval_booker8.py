# 1.Imports
from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException, NoSuchElementException
from datetime import datetime
import time


# Set up the Chrome WebDriver

#2.Setup Chrome WebDriver:
#This line creates a ChromeOptions object, 
#which allows you to set various options for the Chrome driver.
#options = webdriver.ChromeOptions()
chrome_options = Options()
chrome_options.add_experimental_option("debuggerAddress", "localhost:65536")

#This option runs Chrome in headless mode, 
#it will not display a UI or open a browser window.


########options.add_argument("--headless")  # Run in headless mode, without a UI.


#3.Initialize WebDriver:
#This line initializes a new instance of Chrome with the specified options.
driver = webdriver.Chrome(options=chrome_options)



# Navigate to the login page and log in
# 4.Login Sequence:

# Replace these with your actual login credentials and element IDs
username = "hongbaki"
#########################################################
#########################################################
#########################################################
##you need to make one password.txt

with open("../../../password.txt", "r") as password_file:
    password = password_file.read().strip()
#########################################################
#########################################################
#########################################################
username_field_id = "username"  # Replace with the actual ID of the username field
password_field_id = "password"  # Replace with the actual ID of the password field

# Navigate to the login page
driver.get("https://auth.42.fr/auth/realms/students-42/protocol/openid-connect/auth?client_id=intra&redirect_uri=https%3A%2F%2Fprofile.intra.42.fr%2Fusers%2Fauth%2Fkeycloak_student%2Fcallback&response_type=code&state=ae365667127c864fe9576c8eb3f4285649467e597d498f6f")  # Replace with the actual login URL

time.sleep(1)

# Wait for the username field to be present
username_field = driver.find_element(By.ID, username_field_id)
username_field.send_keys(username)

time.sleep(0.5)

# Press Tab to move to the next field
username_field.send_keys(Keys.TAB)
# Find the password input field and enter the password
password_field = driver.find_element(By.ID, password_field_id)


time.sleep(0.5)

password_field.send_keys(password)
username_field.send_keys(Keys.ENTER)








# Navigate to the slots page
#5.Navigate to Slots Page:
#After logging in, the script navigates to the slots page where the available slots are listed.

# Dynamically build the URL
base_url = "https://projects.intra.42.fr/projects"

#########################################################
#########################################################
#########################################################
project_name = "cpp-module-09"  # Replace with the actual project name
#########################################################
#########################################################
#########################################################


#########################################################
#########################################################
#########################################################
#everyone has different team_id
#forexample 
#hongbae has "5374260"
#Ramesh has  "5395823"
team_id = "5374260"  # Replace with the actual team ID 
#########################################################
#########################################################
#########################################################
full_url = f"{base_url}/{project_name}/slots?team_id={team_id}"

# Navigate to the specified slots page
driver.get(full_url)

time.sleep(2)

#########################################################
#########################################################
#########################################################
# Set the desired time for the slot
desired_start_time = datetime.strptime("13:00", "%H:%M").time()  # 24-hour format
desired_end_time = datetime.strptime("23:45", "%H:%M").time()  # 24-hour format
#########################################################
#########################################################
#########################################################

# Function to convert 12-hour format time to 24-hour format
def convert_to_24hr_format(time_str):
    try:
        return datetime.strptime(time_str, "%I:%M %p").strftime("%H:%M")
    except ValueError:
        print(f"Error converting time: {time_str}")
        return None

# Function to check if the slot time is within the desired range
def is_time_within_range(time_str, start_time, end_time):
    try:
        slot_time = datetime.strptime(time_str, "%H:%M").time()  # Expecting 24-hour format
        return start_time <= slot_time <= end_time
    except ValueError as e:
        print(f"Error parsing time: {time_str} - {e}")
        return False

# This flag will indicate whether a slot has been successfully clicked
slot_clicked = False
max_retries = 100000
attempts = 0

while not slot_clicked and attempts < max_retries:
    try:
        attempts += 1
        print(f"Attempt {attempts} of {max_retries}")
        time.sleep(1)

        todays_date_str = datetime.today().strftime("%Y-%m-%d")
        try:         
            #initialize today_colume with 'fc-state-highlight' from html
            today_column = driver.find_element(By.CSS_SELECTOR, f"td.fc-day.fc-widget-content.fc-today.fc-state-highlight[data-date='{todays_date_str}']")       
            html_of_element = today_column.get_attribute('outerHTML')
            available_slots_today = []

            time.sleep(1)
                       
            current_day = datetime.now().weekday()
            xpath = f".//tr/td[{current_day + 2}]//div[contains(@class, 'fc-time')]"
            slots = driver.find_elements(By.XPATH, xpath)

            if (len(slots) == 0):
                driver.refresh()
            print("15")
            
            for slot in slots:
                print("20 : slot.text", slot.text)
                
                time_str = slot.get_attribute("data-full").split(" - ")[0]
                # Debugging: Check the type and value of time_str
                print("21 : time_str:", time_str, "Type:", type(time_str))

                if is_time_within_range(convert_to_24hr_format(time_str), desired_start_time, desired_end_time):
                    print("30 : check time range")
                    available_slots_today.append(slot)

            if not available_slots_today:
                print("No slots available within the desired time range.")
                driver.refresh()
                time.sleep(5)
                continue
            

            for slot in available_slots_today:
                print("40")
                WebDriverWait(driver, 3).until(EC.element_to_be_clickable(slot))
                print("41")
                slot.click()
                print("Clicked on an available slot.")
                slot_clicked = True
                
                time.sleep(5)
                # Find the "OK" button. Adjust the selector as per your page's structure
                try:
                    nextok = driver.find_element(By.CSS_SELECTOR, "button.btn.btn-primary")
                    if nextok.text == "OK":
                        
                        ###########last
                        ###########last
                        ###########last
                        #nextok.click()
                        ###########last
                        ###########last
                        ###########last
                        
                        print("Clicked 'OK' button.")
                except NoSuchElementException:
                    print("OK button not found.")
 
                #break

        except NoSuchElementException:
            print("Today's column is not found or not highlighted.")
            driver.refresh()
            time.sleep(5)

    except TimeoutException:
        print("Timeout occurred while looking for slots. Refreshing and retrying...")
        driver.refresh()
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        break

    time.sleep(2)

if attempts >= max_retries:
    print("Reached the maximum number of retries. Exiting.")


time.sleep(20)

# Close the WebDriver
#8.Close the WebDriver:
#This line closes the browser and ends the WebDriver's session. 
# It's important to include this to free up resources and not leave the browser running in the background.
#driver.quit()



