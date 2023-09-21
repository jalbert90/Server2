let btn = document.getElementById("btn");
let num = document.getElementById("num");

function btnPress() {
    num.innerHTML = Math.floor(Math.random() * 11)
}

btn.addEventListener("click", btnPress);