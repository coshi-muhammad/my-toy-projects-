<script>


  import "./app.css";
  import Task from "./task.svelte"
  let to_dos = $state([]);
  let temporary_value = $state("");
  let show_input_box = $state(false);
  
  function deleteTask(id){
    to_dos = to_dos.filter((todo)=> todo.id != id);
    for(let i = 1; i<to_dos.length;i++){
      to_dos[i].id = i;
    } 
    console.log(id);
  }
   
  function addTask(e){
    if(e.keyCode == 13){
      to_dos.push({value:temporary_value,id:to_dos.length+1});
      show_input_box = !show_input_box;
      temporary_value = "";
    }
  }

</script>

<div>
  <div class="text-center text-6xl font-bold py-4">
      my to-do list
  </div>
  {#each to_dos as todo}
    <div class="">
      <Task  deleted={()=>{deleteTask(todo.id)}} value={todo.value}/>
    </div>
    {:else}
      <p class=" text-xl text-center text-gray-600 my-4"> 
        there are no tasks for to day do you wish to add one 
      </p>
  {/each} 
  <div class=" w-1/2 h-40 flex justify-end ml-auto mr-96">
    {#if show_input_box}
      <input type="text" onkeypress={addTask} bind:value={temporary_value} class=" w-11/12 h-1/2 my-1 mx-2 border-2 border-black rounded-md px-2 py-2 text-3xl bg-gray-100 truncate"> 
    {/if}
    <button disabled={show_input_box} onclick={()=>{
      show_input_box = !show_input_box;
    }} class="flex w-1/12 h-1/3 mx-2 my-2 justify-center items-center border-2 border-black rounded-md ">
      <img src="./add_element.svg" alt="" class=" w-2/3"> 
    </button>
    
  </div>
</div>

<style lang="postcss">


</style>
