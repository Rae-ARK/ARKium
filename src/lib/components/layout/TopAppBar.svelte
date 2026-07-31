<script lang="ts">
</script>

<header class="top-app-bar">
	<div class="brand">
		<img src="/icon.png" alt="ARKium" />
		<span>ARKium</span>
	</div>
</header>

<style>
	.top-app-bar {
		position: sticky;
		top: 0;

		height: 64px;

		display: flex;
		align-items: center;

		padding: 0 20px;

		background: var(--surface);

		border-bottom: 1px solid var(--border);

		z-index: 100;
	}

	.brand {
		display: flex;
		align-items: center;
		gap: 12px;

		font-weight: 700;
		font-size: 1.1rem;
	}

	.brand img {
		width: 36px;
		height: 36px;
		border-radius: 10px;
	}
</style>
