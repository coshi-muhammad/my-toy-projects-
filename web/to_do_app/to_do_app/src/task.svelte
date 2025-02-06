<script>

    let {value , deleted} = $props();
    let complited = $state(false);
    let text_styling = " w-11/12 px-1 py-1 rounded-md truncate text-3xl text-justify bg-stone-200";


</script>

<div class="flex h-1/12 w-1/2 mx-auto px-2 py-2 justify-end border-2 border-black rounded-md bg-gray-100" >

    <p  class={complited ? " text-gray-600 line-through"+text_styling:text_styling}  >
        {value}
    </p>
    <button class=" w-10 h-10  mx-1 my-1 px-1 py-1 border-2 rounded-md border-black" onclick={()=>{complited = !complited}} disabled={complited}>
        <img src="./complited_task.svg" alt="">

    </button>
    <button class=" w-10 h-10 mx-1 my-1  border-2 rounded-md border-black" onclick={deleted}>
        <img src="./delete_element.svg" alt="" >
    </button>

</div>


<style>
</style>
