let btn = document.getElementById("btn");
let num = document.getElementById("num");
let form = document.getElementById("search-form");
let input = document.getElementById("search-form").elements["query"];
let submit = document.getElementById("search-form").elements["send"];
let results = document.getElementById("search-results");

function btnPress() {
    num.innerHTML = Math.floor(Math.random() * 11)
}

function onSubmit(event) {
    event.preventDefault();
    let query = document.querySelector('[name="query"]').value;

    fetch("/?query=" + query).then(response => {
        if (response.ok) {
            // Extract JSON into JSON array of records, where each record is a JSON object.
            // Extract column names.
            // Display column names.
            // Iterate through JSON array and dynamically insert records us JavaScript (something like returning HTML or something).
            // Merge branch and start a new branch.
            // Start learning a framework. ReactJS perhaps?
            // Beautify website. Make super cool.
            // Come back to database and tables later. (Maybe add add functionality.)
            return response.text()
        }
        else {
            throw new Error("Server not ok error");
        }
    }).then(text => {
        results.innerHTML = text;
    }).catch(error => {
        console.error(error);
        alert(error);
    });
}

btn.addEventListener("click", btnPress);
form.addEventListener("submit", onSubmit);