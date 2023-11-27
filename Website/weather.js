// Get references to DOM elements
const dateHeadingElement = document.getElementById('date-heading');
const forecastDataElement = document.getElementById('forecast-data');

// Define an alignment variable (you can change this to 'left', 'right', or 'center')
const alignment = 'right';

// Function to fetch and display weather information
function fetchAndDisplayWeather() {
  // Fetch weather data from the API
  fetch('https://api.weather.gov/gridpoints/HGX/70,95/forecast/hourly')
    .then(response => response.json())
    .then(data => {
      // Extract the hourly forecast data
      const hourlyForecast = data.properties.periods;

      // Get the current time in the user's timezone
      const currentTime = new Date();

      // Calculate the time 1 hour before the current time
      const oneHourBefore = new Date(currentTime.getTime() - 60 * 60 * 1000);

      // Calculate the time 5 hours after the current time
      const fiveHoursAfter = new Date(currentTime.getTime() + 5 * 60 * 60 * 1000);

      // Filter the data for 1 hour before and 5 hours after the current time
      const filteredForecast = hourlyForecast.filter(period => {
        const periodTime = new Date(period.startTime);
        return periodTime >= oneHourBefore && periodTime <= fiveHoursAfter;
      });

      // Display the date as a heading
      dateHeadingElement.textContent = currentTime.toLocaleDateString();

      // Create an HTML table to display the filtered forecast with added padding
      const table = document.createElement('table');
      table.classList.add('styled-table'); // Add a class for styling

      table.innerHTML = `
        <tr>
          <th class="table-heading time-heading" style="margin-left: 20px;">Time</th>
          <th class="table-heading temperature-heading">Temperature</th>
          <th class="table-heading condition-heading">Condition</th>
        </tr>
        ${filteredForecast.map(period => `
          <tr>
            <td>${new Date(period.startTime).toLocaleTimeString([], { hour: 'numeric', minute: '2-digit', hour12: true })}</td>
            <td class="centered-temperature">${period.temperature}°F</td>
            <td>${period.shortForecast}</td>
          </tr>
        `).join('')}
      `;

      // Set text-align for table headers to "center"
      const thElements = table.querySelectorAll('.table-heading');
      thElements.forEach(th => {
        th.style.textAlign = 'center'; // Align column names to the center
      });

      // Set individual margins for specific headers
      const temperatureHeading = table.querySelector('.temperature-heading');
      temperatureHeading.style.marginLeft = '20px'; // Adjust the margin as needed

      const conditionHeading = table.querySelector('.condition-heading');
      conditionHeading.style.marginLeft = '30px'; // Adjust the margin as needed

      // Center-align the temperature data
      const centeredTemperatureElements = table.querySelectorAll('.centered-temperature');
      centeredTemperatureElements.forEach(td => {
        td.style.textAlign = 'center';
      });

      // Set fixed widths for columns
      const tdElements = table.querySelectorAll('td');
      tdElements[0].style.width = '200px'; // Adjust the width as needed
      tdElements[1].style.width = '200px'; // Adjust the width as needed
      tdElements[2].style.width = '200px'; // Adjust the width as needed

      // Clear the forecastDataElement and append the table
      forecastDataElement.innerHTML = '';
      forecastDataElement.appendChild(table);
    })
    .catch(error => {
      // Handle errors
      console.error('Error fetching weather data:', error);
    });
}

// Fetch and display weather information initially
fetchAndDisplayWeather();

// Set up auto-refresh every 5 minutes
setInterval(fetchAndDisplayWeather, 5 * 60 * 1000); // 5 minutes
