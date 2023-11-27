# # Set the path to your web driver executable (e.g., chromedriver, geckodriver, etc.)
# #webdriver_path = r"C:\Users\mugdh\AppData\Local\Programs\Python\Python310\Lib\site-packages\selenium"
# from selenium import webdriver

# # Initialize a headless browser (you can use other browsers like Firefox too)
# options = webdriver.ChromeOptions()
# options.add_argument('--headless')  # Run in headless mode (no GUI)
# driver = webdriver.Chrome(options=options)

# # URL of the website
# url = 'https://alerts.uh.edu/'

# # Open the website
# driver.get(url)

# # Get the full HTML content
# html_content = driver.page_source

# # Print or save the HTML content as needed
# print(html_content)

# # Close the web browser
# driver.quit()
from selenium import webdriver

driver = webdriver.PhantomJS()
driver.get('https://alerts.uh.edu/')

# This will get the initial html - before javascript
html1 = driver.page_source

# This will get the html after on-load javascript
html2 = driver.execute_script("return document.documentElement.innerHTML;")
