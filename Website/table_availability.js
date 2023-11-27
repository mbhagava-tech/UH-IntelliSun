
function changeColor(tableId, newColor) {
    const table = document.getElementById(tableId);
    if (table) {
        table.style.fill = newColor;
    }
}

function changeText(tableId, status) {
    const table_availability = document.getElementById(tableId);
    if(table_availability)
    {
        table_availability.textContent=status
    }
}

function checkstatus(table)
{
    for(var i=0;i<=3;i++)
    {
    table_n=table[i]
    if (table_n==1)
    {
        changeColor("Table"+String(i+1),"rgb(255,0, 0)");
        changeText("Table"+String(i+1)+"_Availability","Currently in Use")
    }
    else
    {
        changeColor("Table"+String(i+1),"rgb(0, 255, 0)");
        changeText("Table"+String(i+1)+"_Availability","Available")
    }
}
}

// Example: Change the color of Table1 to blue after 3 seconds
var table=[1,0,1,0]
setInterval(function () {checkstatus(table);}, 3000);
