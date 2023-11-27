const reserveButton = document.getElementById('reserveButton');
const popup = document.getElementById('popup');
const closePopup = document.getElementById('closePopup');
const dateOptions = document.getElementById('dateOptions');
const nextButton = document.getElementById('nextButton');
const backButton = document.getElementById('backButton');
const step1 = document.getElementById('step1');
const step2 = document.getElementById('step2');

// Function to clear and populate date options
function populateDateOptions() {
    dateOptions.innerHTML = ''; // Clear existing options
    const today = new Date();
    const daysInWeek = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];

    for (let i = 0; i < 7; i++) {
        const date = new Date(today);
        date.setDate(today.getDate() + i);
        const dayOfWeek = daysInWeek[date.getDay()];
        const dateString = date.toDateString();

        const dateOption = document.createElement('div');
        dateOption.innerHTML = `<input type="radio" name="date" value="${dateString}"> ${dayOfWeek}, ${dateString}`;
        dateOptions.appendChild(dateOption);
    }
}

// Show the popup when the Reserve button is clicked
reserveButton.addEventListener('click', function () {
    popup.style.display = 'block';
    populateDateOptions();
});

// Close the popup when the Close button is clicked
closePopup.addEventListener('click', function () {
    popup.style.display = 'none';
});

// Proceed to the next step
nextButton.addEventListener('click', function () {
    step1.style.display = 'none';
    step2.style.display = 'block';
});

// Go back to the previous step
backButton.addEventListener('click', function () {
    step1.style.display = 'block';
    step2.style.display = 'none';
});
