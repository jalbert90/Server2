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

    fetch("/?query=" + query).then(response => { response.text() }).then(text => { console.log(text) });
}

btn.addEventListener("click", btnPress);
form.addEventListener("submit", onSubmit);