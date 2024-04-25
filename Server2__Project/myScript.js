let btn = document.getElementById("btn");
let num = document.getElementById("num");
let form = document.getElementById("search-form");
let input = document.getElementById("search-form").elements["query"];
let submit = document.getElementById("search-form").elements["send"];
let results = document.getElementById("search-results");

function btnPress() {
    num.innerHTML = Math.floor(Math.random() * 11)
}

/*function onSubmit(event) {
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
}*/

// .then(myResolve, myReject) passes myResolve and myReject to executor function of promise (thus calling the executor function). The executor function executes, using the passed
// callbacks in its body.
// new Promise(executor);
// function executor(myRes, myRej) { if this do myRes; else do myRej; }

async function getDatabaseEntries(query) {
    // Can add try catch
    const response = await fetch("/?query=" + query);
    const entries_string = await response.text();
    console.log(entries_string);
    const entries_obj = JSON.parse(entries_string);

    for (const [key, value] of Object.entries(entries_obj)) {
        console.log(`${key}: ${value}`);
    }

    // json array of objects!! don't forget

    console.log(Object.keys(entries_obj));
}

function onSubmit(event) {
    event.preventDefault();
    const query = document.querySelector('[name="query"]').value;
    getDatabaseEntries(query);
    // parse
    // display
}

btn.addEventListener("click", btnPress);
form.addEventListener("submit", onSubmit);