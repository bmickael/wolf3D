/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: stoupin <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/07/06 11:02:06 by stoupin           #+#    #+#             */
/*   Updated: 2017/07/06 11:02:07 by stoupin          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "render.h"

float		dist(t_coord_f a, t_coord_f b)
{
	t_coord_f	delta;

	delta.x = b.x - a.x;
	delta.y = b.y - a.y;
	return (sqrtf(delta.x * delta.x + delta.y * delta.y));
}

static void	calc_columns(t_env *e)
{
	int			x;
	t_coord_f	c_intersect;
	t_column	*c;
	float		angle_x;

	x = -1;
	while (++x < WIDTH)
	{
		c = &(e->scene.columns[x]);
		angle_x = e->angle_x[x] + e->player.angle;
		c->type = find_wall(e, angle_x, &c_intersect, &(c->wall_x_tex));
		c->wall_h_dist = dist(e->player.location, c_intersect) * e->cos_list[x];
		c->wall_min_angle = atanf(-e->player.height / c->wall_h_dist);
		c->wall_max_angle = atanf((e->wall_height - e->player.height)
									/ c->wall_h_dist);
	}
}

void		init_scene(t_env *e)
{
	if (!(e->scene.columns = (t_column*)malloc(sizeof(t_column) * WIDTH)))
		exit(EXIT_FAILURE);
	if (NOSTALGIA_FACTOR == 1)
		e->scene.scene = e->img_string;
	else if (!(e->scene.scene = (t_pix*)malloc(sizeof(t_pix) * WIDTH * HEIGHT)))
		exit(EXIT_FAILURE);
}

#include <pthread.h>

typedef struct			s_TH
{
	int					n;
	t_env				*e;
}						t_TH;

void					*thread_TH(void *arg)
{
	t_env				*e;
	t_coord_f			angle;
	t_coord_i			c;
	int					limit;

	e = ((t_TH *)arg)->e;
	c.y = ((((t_TH *)arg)->n) < 1) ? -1 : (HEIGHT / 2) +
								((((t_TH *)arg)->n - 1) * (floor(HEIGHT / 6)) - 1);
	limit = ((((t_TH *)arg)->n) < 1) ? HEIGHT / 2 : c.y + (HEIGHT / 6) + 1;
	while (++c.y < limit)
	{
		angle.y = e->angle_y[c.y];
		c.x = 0;
		while (c.x < WIDTH)
		{
			angle.x = e->angle_x[c.x] + e->player.angle;
			if (angle.y <= e->scene.columns[c.x].wall_min_angle)
				render_floor(e, c, angle);
			else if (angle.y <= e->scene.columns[c.x].wall_max_angle)
				render_wall(e, c, angle);
			else
				render_sky(e, c, angle);
			c.x++;
		}
	}
	pthread_exit(NULL);
}
/*
void		render_scene(t_env *e)
{
	t_coord_i	c;
	t_coord_f	angle;

	e->sky->pos = (int)((RATIO * WIDTH) * (e->player.angle / (2.f * PI)));
	calc_columns(e);
	c.y = 0;
	while (c.y < HEIGHT)
	{
		angle.y = e->angle_y[c.y];
		c.x = 0;
		while (c.x < WIDTH)
		{
			angle.x = e->angle_x[c.x] + e->player.angle;
			if (angle.y <= e->scene.columns[c.x].wall_min_angle)
				render_floor(e, c, angle);
			else if (angle.y <= e->scene.columns[c.x].wall_max_angle)
				render_wall(e, c, angle);
			else
				render_sky(e, c, angle);
			c.x++;
		}
		c.y++;
	}
	render_sprites(e);
}
*/

void						render_scene(t_env *e)
{
	t_TH					format[NB_CORES];
	pthread_t				thread[NB_CORES];
	int						i;

	e->sky->pos = (int)((RATIO * WIDTH) * (e->player.angle / (2.f * PI)));
//	render_sky(e, e->player.angle);
	calc_columns(e);
	i = -1;
	while (++i < 4)
	{
		format[i].n = i;
		format[i].e = e;
		pthread_create(&thread[i], NULL, thread_TH, &format[i]);
	}
	i = -1;
	while (++i < NB_CORES)
		pthread_join(thread[i], NULL);
	render_sprites(e);
}

void		scene_to_win(t_env *env)
{
	t_coord_i	c_scr;
	t_coord_i	c_scn;
	t_pix		*scene;
	int			i;

	if (NOSTALGIA_FACTOR == 1)
		return ;
	scene = env->scene.scene;
	i = 0;
	c_scr.y = -1;
	while (++c_scr.y < HEIGHT * NOSTALGIA_FACTOR)
	{
		c_scr.x = -1;
		while (++c_scr.x < WIDTH * NOSTALGIA_FACTOR)
		{
			c_scn.x = c_scr.x / NOSTALGIA_FACTOR;
			c_scn.y = c_scr.y / NOSTALGIA_FACTOR;
			env->img_string[i++] = scene[WIDTH * c_scn.y + c_scn.x];
		}
	}
}