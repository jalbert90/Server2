let btn = document.getElementById("btn");
let num = document.getElementById("num");
let form = document.getElementById("search-form");
let input = document.getElementById("search-form").elements["query"];
let submit = document.getElementById("search-form").elements["search"];
let results = document.getElementById("search-results");

function btnPress() {
    num.innerHTML = Math.floor(Math.random() * 11)
}

function onSubmit(event) {
    event.preventDefault();
    let query = document.querySelector('[name="query"]').value;

    fetch("/?search=" + query).then(response => {
        if (response.ok) {
            return response.text()
        }
        else {
            throw new Error("Server not ok error");
        }
    }).then(text => {
        results.innerHTML = text;
    }).catch(error => {
        console.error(error);
    });
}

btn.addEventListener("click", btnPress);
form.addEventListener("submit", onSubmit);